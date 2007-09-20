
#include "main_window.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent)
{
  setupUi(this);
  guiCallBack = new MP_Gui_Callback_c();
  guiCallBack->setActivated();
  guiCallBackDemix = new MP_Gui_Callback_Demix_c();
  guiCallBackDemo = new MP_Gui_Callback_Demo_c();
  dialog  = new Dialog();
  labelOriginalSignal->setText("No wave file selected for original signal");
  labelBook->setText("No book file selected");
  labelDict->setText("No dictionary file selected");
  label_progress->setText("No decompostion");
  label_progressDemix->setText("No decompostion");
  lineEditSeparateValueDemo->setText("0");
  textEditConsol->append(gplText);
  textEditConsolDemix->append(gplText);
  dictOpen = false;
  dictOpenDemo = false;
  dictOpenDemoDefault = false;
}



/* attendre l'arr�t du thread avant de d�truire player */
MainWindow::~MainWindow()
{
  if (guiCallBack)
    {
      delete guiCallBack;
      guiCallBack = NULL;
    }
  if (guiCallBackDemix)
    {
      delete guiCallBackDemix;
      guiCallBackDemix = NULL;
    }
  if (guiCallBackDemo)
    {
      delete guiCallBackDemo;
      guiCallBackDemo = NULL;
    }

}

void MainWindow::on_tabWidget_currentChanged()
{
  if (tabWidget->currentIndex()== 1)
    {
      if (guiCallBack->getActivated())
        {
          guiCallBack->setDesactivated();

        }
      guiCallBackDemix->setActivated();
    }
  else if (tabWidget->currentIndex()== 0)
    {
      if (guiCallBackDemix->getActivated())
        {
          guiCallBackDemix->setDesactivated();
        }
      guiCallBack->setActivated();
    }
  else if (tabWidget->currentIndex()== 2)
    {
      guiCallBackDemo->setActivated();
      if (guiCallBackDemix->getActivated()) guiCallBackDemix->setDesactivated();
      if (guiCallBack->getActivated()) guiCallBack->setDesactivated();

    }

}


/* lecture ou pause */
void MainWindow::on_btnPlay_clicked()
{
  if (NULL!=guiCallBack->signal)
    {
      std::vector<bool> * selectedChannel =  new std::vector<bool>(guiCallBack->signal->numChans, false);
      for (int i = 0 ; i<guiCallBack->signal->numChans ; i++)
        {
          (*selectedChannel)[i] = true;
        }

      if (radioButtonOri->isChecked())guiCallBack->playBaseSignal(selectedChannel,0,0);
      if (radioButtonApprox->isChecked())guiCallBack->playApproximantSignal(selectedChannel,0,0);
      if (radioButtonResi->isChecked())guiCallBack->playResidualSignal(selectedChannel,0,0);
    }
}

void MainWindow::on_btnOpenSigDemo_clicked()
{
  QString panelName = "MPTK GUI: Open Waves files";
  QString fileType ="Wave Files (*.wav);;All Files (*)";
  QString s =dialog->setOpenFileName(panelName, fileType );
  if (!s.isEmpty())
    {
      if (!guiCallBackDemo->coreInit())
        {
          if (guiCallBackDemo->initMpdCore(s, "")== NOTHING_OPENED) dialog->errorMessage("the file named " + s +" isn't a wave file" );
          else labelOriginalSignalDemo->setText(s);
        }
    }

  else dialog->errorMessage("Empty name file");


}
/* arr�t du lecteur */
void MainWindow::on_btnStop_clicked()
{
  guiCallBack->stopPortAudioStream();
}

void MainWindow::on_btnStopDemix_clicked()
{
  guiCallBackDemix->stopPortAudioStream();
}

void MainWindow::on_btnStopDemo_clicked()
{
  guiCallBackDemo->stopPortAudioStream();

}

/* ouvrir une bo�te de dialogue pour s�lectionner un fichier */

void MainWindow::on_btnOpenSig_clicked()
{
  QString panelName = "MPTK GUI: Open Waves files";
  QString fileType ="Wave Files (*.wav);;All Files (*)";
  QString s =dialog->setOpenFileName(panelName, fileType );
  if (!s.isEmpty())
    {
      if (guiCallBack->getBookOpen() == 1)
        {
          if (guiCallBack->initMpdCore(s, labelBook->text())== NOTHING_OPENED) dialog->errorMessage("the file named " + s +" isn't a wave file" );
          else labelOriginalSignal->setText(s);
        }
      else
        {
          if (guiCallBack->initMpdCore(s, "")== NOTHING_OPENED) dialog->errorMessage("the file named " + s +" isn't a wave file" );
          else labelOriginalSignal->setText(s);
        }
    }
  else dialog->errorMessage("Empty name file");


  return;
}

/* ouvrir une bo�te de dialogue pour s�lectionner un fichier */

void MainWindow::on_btnOpenDict_clicked()
{
  QString panelName = "MPTK GUI: Open dictionary";
  QString fileType ="XML Files (*.xml);;All Files (*)";
  QString s =dialog->setOpenFileName(panelName, fileType );
  if (guiCallBack->coreInit())
    {
      if (!s.isEmpty())
        {
          if (guiCallBack->coreInit())guiCallBack->setDictionary(s);
          labelDict->setText(s);
          dictOpen = true;
        }
      else dialog->errorMessage("Empty name file");
    }
  else dialog->errorMessage("Open a signal first");

  return;
}

void MainWindow::on_pushButtonIterateOnce_clicked()
{
  if (guiCallBack->coreInit()&&dictOpen)guiCallBack->iterateOnce();

}

void MainWindow::on_comboBoxNumIter_activated()
{
  if (guiCallBack->coreInit())guiCallBack->setIterationNumber(comboBoxNumIter->currentText().toULong());
  if (guiCallBack->coreInit())guiCallBack->unsetSNR();
}

void MainWindow::on_comboBoxNumIterDemix_activated()
{
  if (guiCallBackDemix->coreInit())guiCallBackDemix->setIterationNumber(comboBoxNumIterDemix->currentText().toULong());
  if (guiCallBackDemix->coreInit())guiCallBackDemix->unsetSNR();
}

void MainWindow::on_comboBoxNumIterDemo_activated()
{
  if (guiCallBackDemo->coreInit())guiCallBackDemo->setIterationNumber(comboBoxNumIterDemo->currentText().toULong());
  if (guiCallBackDemo->coreInit())guiCallBackDemo->unsetSNR();
}

void MainWindow::on_comboBoxSnr_activated()
{
  if (guiCallBack->coreInit())guiCallBack->setSNR(comboBoxSnr->currentText().toDouble());
  if (guiCallBack->coreInit())guiCallBack->unsetIter();
}

void MainWindow::on_comboBoxSnrDemix_activated()
{
  if (guiCallBackDemix->coreInit())guiCallBackDemix->setSNR(comboBoxSnrDemix->currentText().toDouble());
  if (guiCallBackDemix->coreInit())guiCallBackDemix->unsetIter();
}

void MainWindow::on_comboBoxSnrDemo_activated()
{
  if (guiCallBackDemo->coreInit())guiCallBackDemo->setSNR(comboBoxSnrDemo->currentText().toDouble());
  if (guiCallBackDemo->coreInit())guiCallBackDemo->unsetIter();

}



void MainWindow::on_pushButtonIterateAll_clicked()
{
  label_progress->setText("<font color=\"#FF0000\">Decompostion in progress</font>");
  if (guiCallBack->coreInit()&&dictOpen)guiCallBack->iterateAll();
  label_progress->setText("Decompostion ended");
}

void MainWindow::on_pushButtonSaveBook_clicked()
{
  QString panelName = "MPTK GUI: type a name for saving the book";
  QString fileType ="BIN Files (*.bin);;All Files (*)";
  QString s =dialog->setSaveFileName(panelName, fileType );
  lineEditSaveBook->setText(s);
  if (!s.isEmpty())
    {
      if (guiCallBack->coreInit())guiCallBack->saveBook(s);
    }
  else dialog->errorMessage("Empty name file");

}

void MainWindow::on_pushButtonSaveResidual_clicked()
{
  QString panelName = "MPTK GUI: type a name for saving the residual file";
  QString fileType ="Wave Files (*.wav);;All Files (*)";
  QString s =dialog->setSaveFileName(panelName, fileType );
  lineEditSaveResidual->setText(s);
  if (!s.isEmpty())
    {
      if (guiCallBack->coreInit())guiCallBack->saveResidual(s);
    }
  else dialog->errorMessage("Empty name file");

}


void MainWindow::on_pushButtonSaveDecay_clicked()
{
  QString panelName = "MPTK GUI: type a name for saving the residual file";
  QString fileType ="text Files (*.txt);;All Files (*)";
  QString s =dialog->setSaveFileName(panelName, fileType );
  lineEditSaveDecay->setText(s);
  if (!s.isEmpty())
    {
      if (guiCallBack->coreInit())guiCallBack->saveDecay(s);
    }
  else dialog->errorMessage("Empty name file");
}

void MainWindow::on_comboBoxNumIterSavehit_activated()
{
  guiCallBack->setSave(comboBoxNumIterSavehit->currentText().toULong(),lineEditSaveBook->displayText(),lineEditSaveResidual->displayText(),lineEditSaveDecay->displayText());

}

void MainWindow::on_comboBoxNumIterSavehitDemix_activated()
{
  guiCallBackDemix->setSave(comboBoxNumIterSavehitDemix->currentText().toULong(),lineEditSaveBookDemix->displayText(),lineEditSaveResidualDemix->displayText(),lineEditSaveDecayDemix->displayText(), lineEditSaveSequenceDemix->displayText());
}
void MainWindow::on_pushButtonSaveApprox_clicked()
{
  QString panelName = "MPTK GUI: type a name for saving the approximant file";
  QString fileType ="Wave Files (*.wav);;All Files (*)";
  QString s =dialog->setSaveFileName(panelName, fileType );
  lineEditSaveApproximant->setText(s);
  if (!s.isEmpty())
    {
      if (guiCallBack->coreInit())guiCallBack->saveApproximant(s);
    }
  else dialog->errorMessage("Empty name file");

}

void MainWindow::on_pushButtonSaveApproxDemix_clicked()
{

  QString panelName = "MPTK GUI: type a name for saving the approximant file";
  QString fileType ="Wave Files (*.wav);;All Files (*)";
  QString s =dialog->setSaveFileName(panelName, fileType );
  lineEditSaveApproximantDemix->setText(s);
  if (!s.isEmpty())
    {
      if (guiCallBackDemix->coreInit())guiCallBackDemix->saveApprox(s);
    }
  else dialog->errorMessage("Empty name file");
}

void MainWindow::on_pushButtonStopIterate_clicked()
{
  if (guiCallBack->coreInit())guiCallBack->stopIteration();

}

void MainWindow::on_btnOpenbook_clicked()
{
  QString panelName = "MPTK GUI: Choose a book to open";
  QString fileType ="BIN Files (*.bin);;All Files (*)";
  QString s =dialog->setOpenFileName(panelName, fileType );
  int answer =0;
  if (!s.isEmpty())
    {
      if (guiCallBack->getSignalOpen()==0) answer = dialog->questionMessage("Do you want to rebuild the book and substract atom from original signal?");

      if (answer == 2) dialog->errorMessage("Operation canceled");
      else if (answer == 0) guiCallBack->openBook(s);
      else
        {
          if ( answer == 1)
            {
              guiCallBack->openBook(s);
              guiCallBack->subAddBook();
            }

        }

      labelBook->setText(s);
    }
  else dialog->errorMessage("Empty name file");

}

void MainWindow::on_pushButtonSaveBookDemix_clicked()
{
  QString panelName = "MPTK GUI: type a name for saving the book";
  QString fileType ="BIN Files (*.bin);;All Files (*)";
  QString s =dialog->setSaveFileName(panelName, fileType );
  lineEditSaveBookDemix->setText(s);
  if (!s.isEmpty())
    {
      if (guiCallBackDemix->coreInit())guiCallBackDemix->saveBook(s);
    }
  else dialog->errorMessage("Empty name file");

}

void MainWindow::on_btnOpenSigDemix_clicked()
{
  QString panelName = "MPTK GUI: Open Waves files";
  QString fileType ="Wave Files (*.wav);;All Files (*)";
  QString s =dialog->setOpenFileName(panelName, fileType );
  if (guiCallBackDemix->mixer)
    {
      if (!s.isEmpty())
        {
          if (! guiCallBackDemix->openSignal(s) == SIGNAL_OPENED) dialog->errorMessage("Failed to open original signal file");
          guiCallBackDemix->setBookArray();
          guiCallBackDemix->initMpdDemixCore();
          guiCallBackDemix->plugApprox();
          labelOriginalSignalDemix->setText(s);
        }
      else dialog->errorMessage("Empty name file");
    }
  else dialog->errorMessage("Open mixer file first");

  return;
}

void MainWindow::on_btnOpenMixer_clicked()
{
  QString panelName = "MPTK GUI: Choose a mixer file to open";
  QString fileType ="Text Files (*.txt);;XML Files (*.xml);;All Files (*)";
  QString s =dialog->setOpenFileName(panelName, fileType );
  labelBookMixer->setText(s);
  if (!s.isEmpty())
    {
      labelBookMixer->setText(s);
      if (guiCallBackDemix->openMixer(s))
        {
          labelBookMixer->setText(s);
          char buf[3];
          sprintf(buf, "%d", guiCallBackDemix->mixer->numSources);
          labelBookMixeeNbrSources->setText(buf);
        }
      else dialog->errorMessage("Cannot open mixer file");

    }
  else dialog->errorMessage("Empty name file");

}

void MainWindow::readFromStdout(QString message)
{
  // Read and process the data.
  // Bear in mind that the data might be output in chunks.
  textEditConsol->append( message );
}

void MainWindow::on_btnOpenDictDemix_clicked()
{
  QString panelName = "MPTK GUI: Choose a dictionary for each sources";
  QString fileType ="XML Files (*.xml);;All Files (*)";
  QStringList files = dialog->setOpenFileNames(panelName,fileType);
  if (!guiCallBackDemix->mixer) dialog->errorMessage("Open a mixer file first");
  else
    {
      if (files.count() >0 && files.count()==1)
        {
          for (unsigned int i =0; i< guiCallBackDemix->mixer->numSources;i++)
            guiCallBackDemix->addDictToArray(files.at(0),i);
        }
      else
        {
          if (files.count() >0 && files.count()== guiCallBackDemix->mixer->numSources )
            {
              for (unsigned int i =0; i< guiCallBackDemix->mixer->numSources;i++)
                guiCallBackDemix->addDictToArray(files.at(i),i);
            }
          else
            {
              dialog->errorMessage("Please select the same number of dictionary files than the number of sources");
              return;
            }
        }
      guiCallBackDemix->setDictArray();
      labelDictDemix->setText(files.join("/n"));
    }
}

void MainWindow::on_pushButtonIterateAllDemix_clicked()
{
  label_progress->setText("<font color=\"#FF0000\">Decompostion in progress</font>");
  if (guiCallBackDemix->coreInit()&& guiCallBackDemix->getBookOpen()==BOOK_OPENED)guiCallBackDemix->iterateAll();
  label_progress->setText("Decompostion ended");

}

void MainWindow::on_btnPlayDemix_clicked()
{
  if (NULL!=guiCallBackDemix->signal)
    {
      std::vector<bool> * selectedChannel =  new std::vector<bool>(guiCallBackDemix->signal->numChans, false);
      for (int i = 0 ; i<guiCallBackDemix->signal->numChans ; i++)
        {
          (*selectedChannel)[i] = true;
        }

      if (radioButtonOriDemix->isChecked())guiCallBackDemix->playBaseSignal(selectedChannel,0,0);
      if (radioButtonResiDemix->isChecked())guiCallBackDemix->playResidualSignal(selectedChannel,0,0);
    }

}

void MainWindow::on_pushButtonSaveResidualDemix_clicked()
{
  QString panelName = "MPTK GUI: type a name for saving the residual file";
  QString fileType ="Wave Files (*.wav);;All Files (*)";
  QString s =dialog->setSaveFileName(panelName, fileType );
  lineEditSaveResidualDemix->setText(s);
  if (!s.isEmpty())
    {
      if (guiCallBackDemix->coreInit())guiCallBackDemix->saveResidual(s);
    }
  else dialog->errorMessage("Empty name file");

}

void MainWindow::on_pushButtonSaveDecayDemix_clicked()
{
  QString panelName = "MPTK GUI: type a name for saving the decay file";
  QString fileType ="text Files (*.txt);;All Files (*)";
  QString s =dialog->setSaveFileName(panelName, fileType );

  if (!s.isEmpty())
    {
      if (guiCallBackDemix->coreInit())guiCallBackDemix->saveDecay(s);
      lineEditSaveDecayDemix->setText(s);
    }
  else dialog->errorMessage("Empty name file");

}

void MainWindow::on_pushButtonSaveSequenceDemix_clicked()
{
  QString panelName = "MPTK GUI: type a name for saving the decay file";
  QString fileType ="text Files (*.txt);;All Files (*)";
  QString s =dialog->setSaveFileName(panelName, fileType );
  if (!s.isEmpty())lineEditSaveSequenceDemix->setText(s);

}
void MainWindow::on_radioButtonVerbose_toggled()
{
  if (radioButtonVerbose->isChecked ())
    {
      guiCallBack->setVerbose();
      dialog->errorMessage("set verbose");
    }
  else guiCallBack->unSetVerbose();
}

void MainWindow::on_radioButtonVerboseDemix_toggled()
{
  if (radioButtonVerboseDemix->isChecked()) guiCallBackDemix->setVerbose();
  else guiCallBackDemix->unSetVerbose();
}

void MainWindow::on_btnOpenDefaultSig_clicked()
{
  std::string strAppDirectory;
  if (!guiCallBackDemo->coreInit()){
#ifdef __WIN32__
  char szAppPath[MAX_PATH] = "";

  GetModuleFileName(NULL, szAppPath, MAX_PATH);

// Extract directory
  strAppDirectory = szAppPath;

  strAppDirectory = strAppDirectory.substr(0, strAppDirectory.rfind("\\"));
  strAppDirectory += "\\glockenspiel.wav";
  labelOriginalSignalDemo->setText(QString(strAppDirectory.c_str()));
  guiCallBackDemo->initMpdCore(QString(strAppDirectory.c_str()), "");
#else
  char path[2048];
  getcwd(path, 2004);
  strAppDirectory = path;
  strAppDirectory += "/glockenspiel.wav";
  labelOriginalSignalDemo->setText(QString(strAppDirectory.c_str()));
  guiCallBackDemo->initMpdCore(QString(strAppDirectory.c_str()), "");
#endif /* WIN32 */
  }
}

void MainWindow::on_btnValidateDefautlDict_clicked()
{
  std::string strAppDirectory;
  if (guiCallBackDemo->coreInit() && !dictOpenDemo)
    {
#ifdef __WIN32__
      char szAppPath[MAX_PATH] = "";

      GetModuleFileName(NULL, szAppPath, MAX_PATH);

// Extract directory
      strAppDirectory = szAppPath;

      strAppDirectory = strAppDirectory.substr(0, strAppDirectory.rfind("\\"));
      strAppDirectory += "\\dic_gabor_two_scales.xml";
      if (guiCallBackDemo->coreInit())labelDictDemixDemo->setText(QString(strAppDirectory.c_str()));
      if (guiCallBackDemo->coreInit())guiCallBackDemo->setDictionary(QString(strAppDirectory.c_str()));
#else
      char path[2048];
      getcwd(path, 2004);
      strAppDirectory = path;
      strAppDirectory += "/dic_gabor_two_scales.xml";
      labelDictDemixDemo->setText(QString(strAppDirectory.c_str()));
      if (guiCallBackDemo->coreInit())guiCallBackDemo->setDictionary(QString(strAppDirectory.c_str()));
#endif /* WIN32 */
      dictOpenDemoDefault = true;
    }
}


void MainWindow::on_btnLauchDemo_clicked()
{
  if (guiCallBackDemo->coreInit() && (dictOpenDemoDefault||dictOpenDemo))
    {
      //guiCallBackDemo->setIterationNumber(comboBoxNumIterDemo->currentText().toULong());
      guiCallBackDemo->iterateAll();
      if(horizontalScrollBarDemo->value()>0)guiCallBackDemo->separate(horizontalScrollBarDemo->value());
      else guiCallBackDemo->separate(200);
    }
  else dialog->errorMessage("parameter not correctly set");


}

void MainWindow::on_btnPlayDemo_clicked()
{

  if (NULL!=guiCallBackDemo->signal)
    {
      std::vector<bool> * selectedChannel =  new std::vector<bool>(guiCallBackDemo->signal->numChans, false);
      for (int i = 0 ; i<guiCallBackDemo->signal->numChans ; i++)
        {
          (*selectedChannel)[i] = true;
        }

      if (radioButtonOriDemo->isChecked())guiCallBackDemo->playBaseSignal(selectedChannel,0,0);
      if (radioButtonTransiDemo->isChecked())guiCallBackDemo->playTransientSignal(selectedChannel,0,0);
      if (radioButtonOtherDemo->isChecked())guiCallBackDemo->playOtherSignal(selectedChannel,0,0);
      if (radioButtonResiDemo->isChecked())guiCallBackDemo->playResidualSignal(selectedChannel,0,0);
    }

}

void MainWindow::on_horizontalScrollBarDemo_valueChanged(){
if (guiCallBackDemo->coreInit()){
char buf[32];
sprintf(buf, "%f",1000.0*horizontalScrollBarDemo->value()/ guiCallBackDemo->getSignalSampleRate());
lineEditSeparateValueDemo->setText(buf);}

}

void MainWindow::on_btnOpenDictDemo_clicked()
{
  QString panelName = "MPTK GUI: Open dictionary";
  QString fileType ="XML Files (*.xml);;All Files (*)";
  QString s =dialog->setOpenFileName(panelName, fileType );
  if (guiCallBackDemo->coreInit() && !dictOpenDemoDefault)
    {
      if (!s.isEmpty())
        {
          if (guiCallBackDemo->coreInit())guiCallBackDemo->setDictionary(s);
          labelDictDemixDemo->setText(s);
          dictOpenDemo = true;
        }
      else dialog->errorMessage("Empty name file");
    }
  else dialog->errorMessage("Open a signal first");
}
/*********** SLOTS ***********/


/*
 slot appel� lors de la lecture du fichier
 ici on pourra�t faire des mises � jour de l'interface
 ex: avoir un indicateur visuel de la position de la
 lecture en cours sur le fichier
*/

void MainWindow::reader_thread_update()
{}

/*
 * slot appel� lorsque le thread arr�te la lecture suite � la fin
 * du fichier ou d'une demande d'arr�t de l'utilisateur.
 */

void MainWindow::reader_thread_ok()
{}

/*
 * slot appel� lorsque le thread arr�te suite � une
 * erreur: de lecture ou d'arr�t inopin� du thread
 */

void MainWindow::reader_thread_exception()
{
  return;
}



/*********** END SLOTS ***********/
