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

CSV (Comma seperated value) file reader. 
This component reads all columns as attributes into the data memory. One line thereby represents one frame or sample. The first line may contain a header with the feature names (see header=yes/no/auto option).

*/


#ifndef __CCSVSOURCE_HPP
#define __CCSVSOURCE_HPP

#include <core/smileCommon.hpp>
#include <core/dataSource.hpp>

#define COMPONENT_DESCRIPTION_CCSVSOURCE "This component reads CSV (Comma seperated value) files. It reads all columns as attributes into the data memory. One line represents one frame. The first line may contain a header with the feature names (see header=yes/no/auto option)."
#define COMPONENT_NAME_CCSVSOURCE "cCsvSource"

#undef class
class DLLEXPORT cCsvSource : public cDataSource {
  private:
    //int nAttr;
    FILE *filehandle;
    const char *filename;

    int *field;
    //int fieldNalloc;
    int nFields, nCols;
    int eof;

    int header;
    char delimChar;

    long lineNr;
    long start;
    long end;

  protected:
    SMILECOMPONENT_STATIC_DECL_PR
    
    void setGenericNames(int nDelim);
    void setNamesFromCSVheader(char *line, int nDelim);

    virtual void fetchConfig();
    //virtual int myConfigureInstance();
    virtual int myFinaliseInstance();
    virtual int myTick(long long t);

    virtual int setupNewNames(long nEl=0);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cCsvSource(const char *_name);

    virtual ~cCsvSource();
};




#endif // __CCSVSOURCE_HPP
