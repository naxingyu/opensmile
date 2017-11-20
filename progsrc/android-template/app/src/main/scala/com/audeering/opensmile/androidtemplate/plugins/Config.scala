package com.audeering.opensmile.androidtemplate.plugins

import com.audeering.opensmile.androidtemplate.SmilePlugin

/*
 Copyright (c) 2016 audEERING UG. All rights reserved.

 Date: 09.08.16
 Author(s): Gerhard Hagerer
 E-mail:  gh@audeering.com

 Place a short description of what this code does here (1-2 sentences).

 This class contains everything to configure the plugins.
 Each plugin must be declared as an object of the package
 com.audeering.opensmile.androidtemplate.plugins.
 Then, you have to add our own plugin object to the plugin list of the present Config object,
 see plugins attribute below.
 Do not forget to include your openSMILE config files to the assets attribute.
 It is assumed, that you place your configs within the app/src/assets folder.
 Your main config, which includes all the other ones, must be declared explicitly as such by
 defining the mainConf accordingly.
*/

object Config {
  val assets = List(
    "liveinput_android.conf",
    "BufferModeRb.conf.inc",
    "BufferModeLive.conf.inc",
    "messages.conf.inc",
    "music_features.conf.inc",
    "FrameModeMoodWin.conf.inc"
  )
  val mainConf = "liveinput_android.conf"
  val plugins: List[SmilePlugin] = List(Energy)
}
