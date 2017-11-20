package com.audeering.opensmile.androidtemplate.plugins

import android.widget.TextView
import com.audeering.opensmile.androidtemplate.{SmileMessage, SmilePlugin, R}

/*
 Copyright (c) 2016 audEERING UG. All rights reserved.

 Date: 08.08.16
 Author(s): Gerhard Hagerer
 E-mail:  gh@audeering.com

 this is an example plugin showing the energy as number from three bands: treble, middle, bass
*/

object Energy extends SmilePlugin {

  // here is the reference to the layout to display values coming from openSMILE
  // it contains three text fields, one for each band
  override val layoutId = R.layout.fragment_show

  // this is a filter function, which is applie
  override def filter(jsn: SmileMessage) = {
    jsn("msgtype") == "energy" && jsn("msgname") == "energy_act"
  }

  // these attributes hold values calculated by updateValues()
  var bass, middle, treble = 0f

  /**
    * openSMILE sends information to the app in the form of JSON messages.
    * These are
    *
    * @param msg a JSONObject representing an openSMILE message
    */
  override def updateValues(msg: SmileMessage): Unit = {
    treble = msg.floatData(2)
    middle = msg.floatData(1)
    bass   = msg.floatData(0)
  }

  /**
    * this method runs on the UI thread. it should take the object attributes saved in
    * updateValues() and display these on GUI elements.
    */
  override def updateUI() {
    R.id.treble.as[TextView].setText("" + treble)
    R.id.middle.as[TextView].setText("" + middle)
    R.id.bass.as[TextView].setText("" + bass)
  }
}