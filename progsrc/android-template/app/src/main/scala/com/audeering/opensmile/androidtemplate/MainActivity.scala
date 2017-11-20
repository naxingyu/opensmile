package com.audeering.opensmile.androidtemplate

import java.io._

import android.Manifest.permission._
import android.app.ProgressDialog
import android.content.pm.PackageManager._
import android.os.{Bundle, Handler}
import android.support.v4.app._
import android.support.v4.content.ContextCompat
import android.support.v4.view.ViewPager
import android.support.v7.app.AppCompatActivity
import android.util.Log
import android.view._
import com.audeering.opensmile.androidtemplate.plugins.Config

/*
 Copyright (c) 2016 audEERING UG. All rights reserved.

 Date: 08.08.2016
 Author(s): Gerhard Hagerer
 E-mail:  gh@audeering.com

  This is the main activity and the initialization class for this app
*/

class MainActivity extends AppCompatActivity {

  val TAG = classOf[MainActivity].getCanonicalName
  var pagerAdapter: PagerAdapter = null
  var pager: ViewPager = null
  val assets = Config.assets
  def conf = getCacheDir + "/" + Config.mainConf

  val PERMISSION_REQUEST = 0
  val permissions = Array[String](RECORD_AUDIO, WRITE_EXTERNAL_STORAGE, READ_EXTERNAL_STORAGE)

  override def onCreate(state: Bundle) {
    super.onCreate(state)
    setContentView(R.layout.main)
    pagerAdapter = new PagerAdapter(getSupportFragmentManager)
    pager = R.id.pager.as[ViewPager]
    pager.setAdapter(pagerAdapter)
    SmilePlugin
    pager.addOnPageChangeListener(new ViewPager.SimpleOnPageChangeListener{
      override def onPageSelected(pos: Int) {
        SmilePlugin.updateView(activeFragment.get, pos)
      }
    })
    setupAssets
  }

  object SmileThread {
    var isActive = false
    def start {
      (() => {
        isActive = true
        SmileJNI.SMILExtractJNI(conf, 1)
      }).run
    }
    def stop {
      isActive = false
      SmileJNI.SMILEndJNI
    }
  }

  def setMenuItemIcon(item: MenuItem, icon: Int, delay: Int = 2000) {
    (() => {
      item.setEnabled(false)
      item.setVisible(false)
    }).runUi
    (() => {
      item.setIcon(icon)
      item.setEnabled(true)
      item.setVisible(true)
    }).runUiDelayed(delay)
  }

  override def onOptionsItemSelected(item: MenuItem): Boolean = {
    item.getItemId match {
      case R.id.home =>
      case R.id.action_play_stop =>
        if (SmileThread.isActive) {
          SmileThread.stop
          setMenuItemIcon(item, R.drawable.ic_play_arrow_white_24dp)
        }
        else {
          SmileThread.start
          setMenuItemIcon(item, R.drawable.ic_stop_white_24dp)
        }
      case _ =>
    }
    true
  }

  /**
    * show a loading popup while the openSMILE conf-files are being loaded
    * if the required permissions are not granted yet, it covers that, too
    */
  def setupAssets {
    val grants = permissions.map( ContextCompat.checkSelfPermission(this, _) )
    if (grants contains PERMISSION_DENIED) {
      ActivityCompat.requestPermissions(this, permissions, PERMISSION_REQUEST)
    }
    else {
      val dialog = new ProgressDialog(this)
      dialog.setTitle(R.string.asset_dialog_title)
      dialog.setMessage("Please wait...")
      dialog.show()
      (() => {
        assets.foreach(cacheAsset(_))
        (() => dialog.dismiss ).runUi
      }).run
    }
  }


  override def onRequestPermissionsResult(request: Int, permissions: Array[String], grants: Array[Int]) {
    if (request != PERMISSION_REQUEST || Option(grants) == None || grants.contains(PERMISSION_DENIED))
      log("Permission denied")
    else
      log("Permission granted")
    setupAssets
  }

  /**
    * copies a file to a given destination
    *
    * @param filename the file to be copied
    * @param dst destination directory (default: cacheDir)
    */
  def cacheAsset(filename: String, dst: String = getCacheDir.toString) {
    val is = getAssets.open(filename)
    val outfile = new File(dst + "/" + filename)
    outfile.getParentFile.mkdirs()
    val os = new FileOutputStream(outfile)
    val buffer = new Array[Byte](50000)
    Stream.continually(is.read(buffer))
      .takeWhile(_ != -1)
      .foreach(os.write(buffer, 0, _))
    is.close
    os.flush
    os.close
  }

  // START shortcut methods
  def log(str: String) = Log.i(TAG, str)
  def activeFragment: Option[View] = {
    val tag = "android:switcher:" + R.id.pager + ":" + pager.getCurrentItem
    val fragment = getSupportFragmentManager.findFragmentByTag(tag)
    if (Option(fragment) != None)
      return Option(fragment.getView())
    None
  }
  implicit class ViewGetter(id: Int) {
    def as[t] = findViewById(id).asInstanceOf[t]
    def as[t](v: View) = v.findViewById(id).asInstanceOf[t]
  }
  implicit class Closure(func: () => Any) {
    def asRunnable = new Runnable { override def run() = func() }
    def asThread   = new Thread(asRunnable)
    def run        = asThread.start()
    def runUi      = runOnUiThread(asRunnable)
    def runUiDelayed(delay: Int) = new Handler().postDelayed(func.asRunnable, delay)
  }
  // END shortcut methods

  /**
    * this adds items like buttons to the action bar
    */
  override def onCreateOptionsMenu(menu: Menu) = {
    getMenuInflater.inflate(R.menu.toolbar, menu)
    super.onCreateOptionsMenu(menu)
  }

  class PagerAdapter(fm: FragmentManager) extends FragmentPagerAdapter(fm) {
    override def getItem(pos: Int) = new SlideFragment(pos)
    override def getCount = SmilePlugin.layoutsNum
    override def instantiateItem(container: ViewGroup, pos: Int): Object = {
      val obj = super.instantiateItem(container, pos)
      if (obj.isInstanceOf[Fragment] && pos == 0 && SmilePlugin.activeView == None) {
        SmilePlugin.updateView(container, 0)
      }
      obj
    }
  }

  class SlideFragment(val pos: Int) extends Fragment {
    override def onCreateView(inflater: LayoutInflater, container: ViewGroup, state: Bundle) = {
      inflater.inflate(SmilePlugin.layouts(pos), container, false)
    }
  }
}
