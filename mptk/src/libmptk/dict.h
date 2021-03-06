/******************************************************************************/
/*                                                                            */
/*                                 dict.h                                     */
/*                                                                            */
/*                        Matching Pursuit Library                            */
/*                                                                            */
/* R�mi Gribonval                                                             */
/* Sacha Krstulovic                                           Mon Feb 21 2005 */
/* -------------------------------------------------------------------------- */
/*                                                                            */
/*  Copyright (C) 2005 IRISA                                                  */
/*                                                                            */
/*  This program is free software; you can redistribute it and/or             */
/*  modify it under the terms of the GNU General Public License               */
/*  as published by the Free Software Foundation; either version 2            */
/*  of the License, or (at your option) any later version.                    */
/*                                                                            */
/*  This program is distributed in the hope that it will be useful,           */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/*  GNU General Public License for more details.                              */
/*                                                                            */
/*  You should have received a copy of the GNU General Public License         */
/*  along with this program; if not, write to the Free Software               */
/*  Foundation, Inc., 59 Temple Place - Suite 330,                            */
/*  Boston, MA  02111-1307, USA.                                              */
/*                                                                            */
/******************************************************************************/

/***************************************/
/*                                     */
/* DEFINITION OF THE DICTIONNARY CLASS */
/*                                     */
/***************************************/
/*
 * SVN log:
 *
 * $Author: broy $
 * $Date: 2007-07-13 16:21:09 +0200 (Fri, 13 Jul 2007) $
 * $Revision: 1109 $
 *
 */

#include "mp_pthreads_barrier.h"
#include "block_factory.h"
#include "tinyxml.h"
#include <string>
#include <list>
#include <map>
#include <vector>



#ifndef __dict_h_
#define __dict_h_


/***********************/
/* DICT CLASS          */
/***********************/
/** \brief Dictionaries are unions of several blocks used to perform Matching Pursuit.
 */
class MP_Dict_c
  {

    /** \Self pointer on Mp_dict_c class to pass it as an object in the parallel construct.
     */
    typedef MP_Dict_c Self;
    /********/
    /* DATA */
    /********/

  public:
    /** \brief Signal manipulated by the dictionnary */
    MP_Signal_c *signal;
    /** \brief The "mode" of the signal: either the signal was copied into the dictionary,
     * or it was just referenced. */
    int sigMode;
    /** \brief The various possible signal modes: */
#define MP_DICT_NULL_SIGNAL     0
#define MP_DICT_EXTERNAL_SIGNAL 1 /* => don't delete the sig when deleting the dict */
#define MP_DICT_INTERNAL_SIGNAL 2 /* => delete the sig when deleting the dict */

    /** \brief Number of blocks stored in the dictionary */
    unsigned int numBlocks;
    /** \brief Storage space : an array of [numBlocks] block objects */
    MP_Block_c** block;
    /** \brief Index in the storage space of the block which holds the max inner product */
    unsigned int blockWithMaxIP;
    /** \brief Tab of MP_Real_t to stock the result of the threads computation */
    volatile MP_Real_t* val;
    /** \brief Multi-channel support that identifies which part of the signal is different from
     * what it was when the blocks were last updated 
     */
    MP_Support_t* touch;
    
    /** \brief Length of the longest atom of the dictionary
     */
    unsigned long int maxFilterLen;

    /** \brief Map use to pass the block creation properties */

    typedef map<string, string, mp_ltstring> PropertiesMap;

    /** \brief Map use to stock the various PropertiesMap with properties name */

    map<string, PropertiesMap, mp_ltstring> propertyMap;

    /** \brief Struct use to pass the data and the function to the threads for parallel computation */

    struct ParallelConstruct
      {
        /** \brief the block number (same than thread number)
         */
        int blocknumber;

        /** \brief Local adress of the MP_Real_t to write for each threads
         */
        volatile MP_Real_t* val;

        /** \brief Self pointer to pass object context to the threads
         */
        Self* parent;

        /** \brief boolean defining is the thread should exit
         */
        bool exit ;

        /** \brief Static function to run the threads and pass the object context via the self pointer
         */
        static void* c_run(void* a)
        {
          /* Static cast */
          ParallelConstruct* f = static_cast<ParallelConstruct*>(a);
          /* Call the the function to compute in parallel */
          f->parent->calcul_max_per_block(f);

          return 0;
        }
      };
    int b;

    /** \brief tab of thread*/
    pthread_t *threads;

    /** \brief tab of task to pass to the threads   */
    ParallelConstruct *tasks;

    /** \brief tab of pointer on barrier for synchronizing the thread*/
    MP_Barrier_c **bar;



    /***********/
    /* METHODS */
    /***********/

    /***************************/
    /* CONSTRUCTORS/DESTRUCTOR */
    /***************************/

  public:
    /** \brief Factory function which reads the dictionary from a file
     *
     * \param dictFileName The name of the XML file parsed for a dictionary.
     *
     * WARNING: this function does not set a signal in the dictionary.
     * It is mandatory to call dict.copy_signal( signal ) or
     * dict.plug_signal( signal ) before starting to iterate.
     */
	  MPTK_LIB_EXPORT static MP_Dict_c* read_from_xml_file( const char* dictFileName );
    /** \brief Factory function which reads the dictionary from a file
     *
     * \param fid A readable file descriptor where XML description of the dictionary will be read.
     *
     * WARNING: this function does not set a signal in the dictionary.
     * It is mandatory to call dict.copy_signal( signal ) or
     * dict.plug_signal( signal ) before starting to iterate.
     */
	  MPTK_LIB_EXPORT static MP_Dict_c* read_from_xml_file(FILE *fid);

    /** \brief Factory function which creates an empty dictionary.
     *
     * This function makes it possible to create an empty dictionary
     * and then independently call dict.add_blocks( fileName ) and
     * dict.copy_signal( signal ) or dict.plug_signal( signal ).
     *
     */
   MPTK_LIB_EXPORT static MP_Dict_c* init( void );

  protected:
    /* NULL constructor */
    MPTK_LIB_EXPORT MP_Dict_c();


  public:
    /* Destructor */
    MPTK_LIB_EXPORT virtual ~MP_Dict_c();


    /***************************/
    /* I/O METHODS             */
    /***************************/

    /** \brief Add blocks to a dictionary by loading them from an XML file.
     *
     * \param fName A string containing the file name
     * \return the number of added/loaded blocks
     */
	  MPTK_LIB_EXPORT int add_blocks_from_xml_file(const char* fName);
    /** \brief Add blocks to a dictionary by loading them from an XML file.
     *
     * \param fid A readable file descriptor
     * \return the number of added/loaded blocks
     */
	  MPTK_LIB_EXPORT int add_blocks_from_xml_file(FILE *fid);
    
    /** \brief Add blocks to a dictionary by loading them from a TinyXML doc.
    *
    *  \param doc A TiXmlDocument
     * \return the number of added/loaded blocks
    */
	  MPTK_LIB_EXPORT int add_blocks_from_xml_doc(TiXmlDocument doc);

    /** \brief Writes the dictionary description to an xml file
     *
     * \param fName A string containing the xml file name.
     * \return zero if succed.
     */
    MPTK_LIB_EXPORT int write_to_xml_file( const char *fName );
    /** \brief Writes the dictionary description to an xml file
     *
     * \param fid A writable file descriptor
     * \return true if succeed, false otherwise.
     */
	MPTK_LIB_EXPORT bool write_to_xml_file(FILE *fid);
    /** \brief Writes the dictionary description to an xml file
     *
     * \param fid A writable file descriptor
     * \param withXmlDecl true if XML declaration (ISO-8859 ...) to be included, false otherwise  
     * \return true if succeed, false otherwise.
     */
	MPTK_LIB_EXPORT bool write_to_xml_file(FILE *fid, bool withXmlDecl);

    /** \brief Append's the dictionary block descriptions to an xml root structure
     *
     * \param root The root of TiXML tree  
     * \return true if succeed, false otherwise.
     */
	MPTK_LIB_EXPORT bool append_blocks_to_xml_root( TiXmlElement* root );

    /** \brief Add a block to a dictionary
     *
     * \param newBlock the reference of the block to add.
     *
     * \return the number of added blocks, i.e. 1 if the block
     * has been successfully added, or 0 in case of failure or
     * if newBlock was initially NULL.
     */
    MPTK_LIB_EXPORT int add_block( MP_Block_c *newBlock );
  
    /** \brief Add a  default blocks to a dictionary.
     * \param blockName A string containing default block name to add
     * \return the number of added blocks, i.e. 1 if the block
     * has been successfully added, or 0 in case of failure or
     * if newBlock was initially NULL.
     */
    MPTK_LIB_EXPORT int add_default_block( const char* blockName );

    /***************************/
    /* MISC METHODS            */
    /***************************/

    /** \brief tiny XML related method
    */

    /** \brief Parse the properties xml block and load it into a map of properties map
    *
    * The properties map stocks the history of all property blocks with a ID in order to
    * use this properties map if a block request it.
    *
    * \param pParent The TiXmlNode to parse.
    * \param setPropertyMap a map<const char*, PropertiesMap, ltstr> contenaing all the properties map with their ID
    * \return true in case of success
    */
    MPTK_LIB_EXPORT bool parse_property(TiXmlNode * pParent, map<string, PropertiesMap, mp_ltstring> *setPropertyMap);

    /** \brief Create a block using the factory method
    *
    *
    * \param setPropertyMap a map containning the parameter of the block to create
    * \param setSignal a a signal to plug the block
    *
    *
    * \return the number of created block
    */
    MPTK_LIB_EXPORT int add_block_from_property_map(MP_Signal_c* setSignal , map<string, string, mp_ltstring>* setPropertyMap);
    
    /** \brief Parse the xml block and create it using eventually a map of properties stocked in the properties map
    *
    *
    * \param pParent The TiXmlNode to parse.
    * \param setPropertyMap a map<const char*, PropertiesMap, ltstr> contenaing all the properties map with their ID
    *
    *
    * \return the number of created block
    */
    MPTK_LIB_EXPORT int parse_block(TiXmlNode * pParent , map<string, PropertiesMap, mp_ltstring> *setPropertyMap);

    /** \brief Parse the  parameter list in xml block and create each block using a different parameter of the list
    *  and eventually a map of properties stocked in the properties map
    * 
    * This method is called recursively.
    *
    * \param setPropertyMap a map<const char*, PropertiesMap, ltstr> contenaing all the properties map with their ID
    * \param setVarParam a map map<string, list<string>, mp_ltstring> containing the var parameter with global parameter
    *
    * \return the number of created block
    */
    MPTK_LIB_EXPORT int parse_param_list(map<string, list<string>, mp_ltstring> setVarParam , map<string, string, mp_ltstring> *setPropertyMap);

    /** \brief Test
     */
    MPTK_LIB_EXPORT static bool test( char* signalFileName, char* dicoFileName );

    /** \brief Get the number of atoms of the dictionary */
    MPTK_LIB_EXPORT unsigned long int num_atoms( void );


    /** \brief Copy a signal into the dictionary
     *
     * The dictionary will act on a local copy of the passed signal
     * (equivalent to a pass by value). Any previously present signal
     * will be replaced.
     *
     * \param setSignal the signal to store into the dictionary.
     * (NULL resets any signal-related info in the dictionary.)
     *
     * \return nonzero in case of failure, zero otherwise.
     */
    MPTK_LIB_EXPORT int copy_signal( MP_Signal_c *setSignal );

    /** \brief Copy a new signal from a file to the dictionary
     *
     * The dictionary will act on a local copy of the passed signal
     * (equivalent to a pass by value). Any previously present signal
     * will be replaced.
     *
     * \param fName the name of the file where to read the stored signal
     *
     * \return nonzero in case of failure, zero otherwise.
     */
    MPTK_LIB_EXPORT int copy_signal( const char *fName );

    /** \brief Plug (or hook) a new signal to the dictionary
     *
     * The dictionary will act directly on the passed signal
     * (equivalent to a pass by reference). Any previously present signal
     * will be replaced.
     *
     * \param setSignal the signal to reference into the dictionary
     * (NULL resets any signal-related info in the dictionary.)
     *
     * \return nonzero in case of failure, zero otherwise.
     */

    MPTK_LIB_EXPORT int plug_signal( MP_Signal_c *setSignal );

    /** \brief Detach the signal from the dictionary, and reset
     * the dictionary signal to NULL.
     *
     * \return The signal which was formerly plugged/copied
     * into the dictionary.
     */
    MPTK_LIB_EXPORT MP_Signal_c* detach_signal( void );


  private:
    /** \brief Allocate the touch array according to the signal size */
    int alloc_touch( void );

  public:

    /** \brief parallel method with ParallelConstruct (tasks) argument */
    MPTK_LIB_EXPORT void calcul_max_per_block(ParallelConstruct* f);

    /** \brief parallel method with void argument */
    MPTK_LIB_EXPORT void calcul_max_per_block(void);

    /** \brief method start a parrallel computing */
    MPTK_LIB_EXPORT void parallel_computing(volatile MP_Real_t* tab);

    /** \brief Delete all the blocks from a dictionary
     *
     * \return The number of deleted blocks
     */
    MPTK_LIB_EXPORT int delete_all_blocks( void );


    /** \brief Compute all the inner products which need to be updated and finds the max.
     *
     * Side-effect : blockWithMaxIP is updated.
     * \return The value of the maximum inner product
     */
    MPTK_LIB_EXPORT MP_Real_t update( void );
    
    /** \brief Compute all the inner products which need to be updated and finds the max.
     * Keep track of some of them in the sub-book.
     *
     * Side-effect : blockWithMaxIP is updated.
     * \return The value of the maximum inner product
     */
    MPTK_LIB_EXPORT MP_Real_t update (GP_Block_Book_c*);

    /** \brief Compute all the inner products and finds the max
     *
     * Side-effect : blockWithMaxIP is updated.
     * \return The value of the maximum inner product
     */
    MPTK_LIB_EXPORT MP_Real_t update_all( void );


    /** \brief create a new atom corresponding to the best atom of the best block.
     *
     * \param atom a pointer to (or an array of) reference to the returned atom(s)
     * \return the number of extracted atom */
    MPTK_LIB_EXPORT unsigned int create_max_atom( MP_Atom_c** atom );
    MPTK_LIB_EXPORT unsigned int create_max_gp_atom( MP_Atom_c** atom );

    /** \brief Perform one matching pursuit iteration (update the blocks, find the max,
     * create the atom and append it to the book, and substract it from the analyzed signal).
     *
     * \param book The book where to append the selected atom
     * \param sigRecons A signal where to add the selected atom for online reconstruction
     * \return one upon success, zero otherwise
     * \remark Pass sigRecons == NULL to skip the reconstruction step
     * \remark Ideally, the number of arithmetic operations is kept to a minimum.
     */
    MPTK_LIB_EXPORT int iterate_mp( MP_Book_c* book , MP_Signal_c* sigRecons );
    MPTK_LIB_EXPORT int iterate_cmp( MP_Book_c* book , MP_Signal_c* sigRecons, int atomIndex );
    MPTK_LIB_EXPORT int iterate_cmphold( MP_Book_c* book , MP_Signal_c* sigRecons, int atomIndex );


  };


#endif /* __dict_h_ */
