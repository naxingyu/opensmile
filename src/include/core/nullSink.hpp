/*F***************************************************************************
 * openSMILE - the open-Source Multimedia Interpretation by Large-scale
 * feature Extraction toolkit
 * 
 * (c) 2008-2011, Florian Eyben, Martin Woellmer, Bjoern Schuller: TUM-MMK
 * 
 * (c) 2012-2013, Florian Eyben, Felix Weninger, Bjoern Schuller: TUM-MMK
 * 
 * (c) 2013-2014 audEERING UG, haftungsbeschränkt. All rights reserved.
 * 
 * Any form of commercial use and redistribution is prohibited, unless another
 * agreement between you and audEERING exists. See the file LICENSE.txt in the
 * top level source directory for details on your usage rights, copying, and
 * licensing conditions.
 * 
 * See the file CREDITS in the top level directory for information on authors
 * and contributors. 
 ***************************************************************************E*/


/*  openSMILE component: cNullSink
----------------------------------
NULL sink: reads data vectors from data memory and destroys them without writing them anywhere
----------------------------------

11/2009  Written by Florian Eyben
*/


#ifndef __CNULLSINK_HPP
#define __CNULLSINK_HPP

#include <core/smileCommon.hpp>
#include <core/dataSink.hpp>

#define COMPONENT_DESCRIPTION_CNULLSINK "This component reads data vectors from the data memory and 'destroys' them, i.e. does not write them anywhere. This component has no configuration options."
#define COMPONENT_NAME_CNULLSINK "cNullSink"

class cNullSink : public cDataSink {
  private:
    //int lag;    

  protected:
    SMILECOMPONENT_STATIC_DECL_PR
    
    virtual int myTick(long long t);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cNullSink(const char *_name);
    virtual ~cNullSink();
};




#endif // __CNULLSINK_HPP
