package com.audeering.opensmile.androidtemplate

import android.app.Activity
import android.view.View
import com.audeering.opensmile.androidtemplate.plugins.{Config, Energy}

import scala.collection.mutable.ListBuffer

/*
 Copyright (c) 2016 audEERING UG. All rights reserved.

 Date: 08.08.2016
 Author(s): Gerhard Hagerer
 E-mail:  gh@audeering.com

  Here classes for receiving and showing the output values from openSMILE can be defined
  as plugins
*/


object SmilePlugin extends SmileJNI.Listener {

  var activePage = 0
  var activeView: Option[View] = None
  val fragments = ListBuffer[Int]()
  val plugins = Config.plugins

  SmileJNI.registerListener(this)

  def updateView(v: View, id: Int): Unit = {
      activeView = Option(v)
      activePage = id
  }

  /**
    * this gets called when openSMILE sends a message to the app
    *
    * @param text JSON encoded string
    */
  override def onSmileMessageReceived(text: String) {
    // at first parse JSON
    val msg = new SmileMessage(text)
    val msgtype = msg("msgtype")
    val msgname = msg("msgname")

    // now see which SmilePlugins are there for that kind of openSMILE message (filtering)
    // and iterate over all SmilePlugins and execute their values
    plugins.filter( _.filter(msg) ).foreach( _.updateValues(msg) )

    // now only take the plugin which is visible and call its ui update (don't forget VAD)
    runUI(() => {
      val visiblePlugin = plugins.filter( _.layoutId == plugins(activePage).layoutId )(0)
      visiblePlugin.updateUI
    })
  }

  def runUI(func: () => Unit) {
    val act = activeView.get.getContext.asInstanceOf[Activity]
    act.runOnUiThread(new Runnable {
      override def run(): Unit = func()
    })
  }
  def view[t](id: Int) = activeView.get.findViewById(id).asInstanceOf[t]
  def layoutsNum = plugins.size
  def layouts = plugins.map(_.layoutId)
}

trait SmilePlugin {
  val layoutId: Int
  def updateValues(values: SmileMessage)
  def updateUI()
  def runUI(func: () => Unit) = SmilePlugin.runUI(func)
  def filter(jsn: SmileMessage): Boolean = true
  implicit class idToView(id: Int) {
    def as[t]: t = SmilePlugin.view[t](id)
  }
}