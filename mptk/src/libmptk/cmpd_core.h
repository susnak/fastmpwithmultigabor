/******************************************************************************/
/*                                                                            */
/*                                cmpd_core.h                                 */
/*                                                                            */
/*                        Matching Pursuit Utilities                          */
/* Benjamin Roy                                                               */
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
/* DEFINITION OF THE CMPD_CORE CLASS   */
/*                                     */
/***************************************/
/*
 * SVN log:
 *
 * $Author: sacha $
 * $Date: 2007-05-24 16:50:42 +0200 (Thu, 24 May 2007) $
 * $Revision: 1053 $
 *
 */

#include "mptk.h"

#ifndef __cmpd_core_h_
#define __cmpd_core_h_

// defaults, used by e.g. cpmd commmandline, and pymptk:
#define MPD_DEFAULT_NUM_ITER   ULONG_MAX
#define CMPD_DEFAULT_MAXNUM_CYCLES   1;
#define CMPD_DEFAULT_MIN_CYCLEIMPROVEDB   0.001;
#define CMPD_DEFAULT_MAXNUM_AUG_BEFORECYCLE   1;
#define CMPD_DEFAULT_MAXNUM_AUG_BEFORECYCLE_DB   0;
#define CMPD_DEFAULT_MAX_AUG_STOPCYCLE   ULONG_MAX;
#define CMPD_DEFAULT_MAX_DB_STOPCYCLE   60.0;


/***********************/
/* CMPD_CORE CLASS      */
/***********************/
/** \brief The MP_CMpd_Core_c class implements a standard run of
 *  the Cyclic Matching Pursuit Decomposition (cmpd) utility.
 */
class MP_CMpd_Core_c:public MP_Mpd_Core_c
  {

    /********/
    /* DATA */
    /********/

  public:
	  unsigned int num_cycles, max_iter_beforecycle, max_iter_stopcycle, itersincelastcycle, holdatoms;
      float min_cycleimprovedB, min_dB_beforecycle, max_dB_stopcycle, residualEnergyAfterCycle;
	  
  private:

    /***********/
    /* METHODS */
    /***********/

    /***************************/
    /* CONSTRUCTORS/DESTRUCTOR */
    /***************************/
  public:
    /** \brief A factory function for the MP_CMpd_Core_c
     * \param signal the signal to decompose
     * \param setBook the book to stock the atoms
     */
    MPTK_LIB_EXPORT static MP_CMpd_Core_c* create( MP_Signal_c *signal, MP_Book_c *setBook );
    /** \brief A factory function for the MP_CMpd_Core_c
     * \param signal the signal to decompose
     * \param setBook the book to stock the atoms
     * \param setDict the dict to rule the decomposition
     */
    MPTK_LIB_EXPORT static MP_CMpd_Core_c* create( MP_Signal_c *setSignal, MP_Book_c *setBook, MP_Dict_c *setDict );
    /** \brief A factory function for the MP_CMpd_Core_c
    * \param setSignal the signal to decompose
    * \param setBook the book to stock the atoms
    * \param setApproximant  an approximant to reconstruct the signal
    * 
    */
    MPTK_LIB_EXPORT static MP_CMpd_Core_c* create( MP_Signal_c *setSignal, MP_Book_c *setBook, MP_Signal_c* setApproximant );
    /** \brief A factory function for the MP_CMpd_Core_c
     * \param setSignal the signal to decompose
     * \param setBook the book to stock the atoms
     * \param setDict the dict to rule the decomposition
     * \param setApproximant  an approximant to reconstruct the signal
     * 
     */
    MPTK_LIB_EXPORT static MP_CMpd_Core_c* create( MP_Signal_c *setSignal, MP_Book_c *setBook, MP_Dict_c *setDict, MP_Signal_c* setApproximant );

  private:
    /** \brief a private constructor */
    MPTK_LIB_EXPORT MP_CMpd_Core_c();

  public:
    /** \brief a public destructor */
    MPTK_LIB_EXPORT virtual ~MP_CMpd_Core_c();

    /***************************/
    /* I/O METHODS             */
    /***************************/
    void set_settings( int inputnum_cycles, float inputmin_cycleimprovedB, int inputmax_iter_beforecycle, float inputmin_dB_beforecycle, 
                      int inputmax_iter_stopcycle, float inputmax_dB_stopcycle, int inputCMPD_HOLD ) {
      //  cout << inputnum_cycles << "\n";
	  num_cycles =  inputnum_cycles;
      min_cycleimprovedB = (float) pow(10.0,inputmin_cycleimprovedB/10.0);
      max_iter_beforecycle = inputmax_iter_beforecycle;
      min_dB_beforecycle = (float) pow(10.0,inputmin_dB_beforecycle/10.0);
      max_iter_stopcycle = inputmax_iter_stopcycle;
      max_dB_stopcycle = (float) pow(10.0,inputmax_dB_stopcycle/10.0);
      itersincelastcycle = 0;
      residualEnergyAfterCycle = (float) ULONG_MAX;
      holdatoms = inputCMPD_HOLD;
	}
	  
  public:

    /***************************/
    /* OTHER METHODS           */
    /***************************/

    /* Control object*/

    /* Runtime */
    /** \brief make one step iterate
     * 
    *  */
    MPTK_LIB_EXPORT unsigned short int step();
    
  };

#endif /* __cmpd_core_h_ */
