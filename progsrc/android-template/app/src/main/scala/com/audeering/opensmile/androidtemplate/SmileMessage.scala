package com.audeering.opensmile.androidtemplate

import org.json.{JSONArray, JSONObject}

import scala.collection.mutable

/*
 Copyright (c) 2016 audEERING UG. All rights reserved.

 Date: 08.08.2016
 Author(s): Gerhard Hagerer
 E-mail:  gh@audeering.com

  This is for parsing a text message coming from openSMILE JNI in JSON format
  Additional helper methods are provided too for easier access to standard attributes
*/


class SmileMessage(jsn: String) extends JSONObject(jsn) {
  def floatData(i: Int) = getJSONObject("floatData").getDouble(""+i).toFloat
  def intData(i: Int) = getJSONObject("floatData").getInt(""+i)
  def apply(key: String) = getString(key)
}