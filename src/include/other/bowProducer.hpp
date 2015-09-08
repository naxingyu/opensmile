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


/*  openSMILE component:

example dataSource
writes data to data memory...

*/


#ifndef __CBOWPRODUCER_HPP
#define __CBOWPRODUCER_HPP

#include <core/smileCommon.hpp>
#include <core/dataSource.hpp>

#define BUILD_COMPONENT_BowProducer
#define COMPONENT_DESCRIPTION_CBOWPRODUCER "This component produces a bag-of-words vector from the keyword spotter result message."
#define COMPONENT_NAME_CBOWPRODUCER "cBowProducer"

#undef class
class DLLEXPORT cBowProducer : public cDataSource {
  private:
    const char *kwList;
    const char *prefix;
    const char *textfile;
    const char *singleSentence;
    int count;
    int dataFlag;
    int kwListPrefixFilter;
    int syncWithAudio;

    int numKw;
    char ** keywords;
    
    FILE * filehandle;
    long lineNr;
    char * line;
    size_t line_n;
    int txtEof;

    int loadKwList();
    int buildBoW( cComponentMessage *_msg );
    int buildBoW( const char * str );

  protected:
    SMILECOMPONENT_STATIC_DECL_PR
    
    virtual void fetchConfig();
    //virtual int myConfigureInstance();
    //virtual int myFinaliseInstance();
    virtual int processComponentMessage( cComponentMessage *_msg );
    virtual int myTick(long long t);

    virtual int configureWriter(sDmLevelConfig &c);
    virtual int setupNewNames(long nEl);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cBowProducer(const char *_name);

    virtual ~cBowProducer();
};




#endif // __CBOWPRODUCER_HPP
