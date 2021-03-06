#include "pyMPTK.h"

void
book_dealloc(BookObject* pybook)
{
	pybook->ob_type->tp_free((PyObject*)pybook);
}

PyObject *
book_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	MPTK_Env_c::get_env()->load_environment_if_needed("");
	BookObject *pybook;

	pybook = (BookObject *)type->tp_alloc(type, 0);
	// NB the values below, pybook_from_mpbook() will overwrite them iff 0 so we should leave them here as 0
	pybook->mpbook = MP_Book_c::create(0, 0, 0);
	pybook->atoms = PyList_New(0);

	return (PyObject *)pybook;
}

int
book_init(BookObject *pybook, PyObject *args, PyObject *kwds)
{
	return 0;
}

PyObject *
book_read(BookObject* pybook, PyObject *args)
{
	char *filename;

	if (!PyArg_ParseTuple(args, "s", &filename))
		return NULL;

	pybook->mpbook->load( filename );

	int result = pybook_from_mpbook(pybook, pybook->mpbook);
	return Py_BuildValue("i", result);
}

int
pybook_from_mpbook(BookObject* pybook, MP_Book_c *mpbook)
{
	int numAtoms = mpbook->numAtoms;
	int n;
	if(PyList_Size(pybook->atoms) != 0){
		PyErr_SetString(PyExc_RuntimeError, "Attempted to load mpbook data into a pybook which is not empty.\n");
		return 4;
	}
	if(pybook->numChans==0){
		pybook->numChans = mpbook->numChans;
	}else if(pybook->numChans != mpbook->numChans ){
		return 1;
	}
	if(pybook->numSamples==0){
		pybook->numSamples = mpbook->numSamples;
	}else if((unsigned long int)pybook->numSamples != mpbook->numSamples ){
		return 2;
	}
	if(pybook->sampleRate==0){
		pybook->sampleRate = mpbook->sampleRate;
	}else if(pybook->sampleRate != mpbook->sampleRate ){
		return 3;
	}
	for ( n=0 ; n<numAtoms ; ++n ) {
		PyObject* atom = pyatom_from_mpatom(mpbook->atom[n], pybook->numChans);
		PyList_Append(pybook->atoms, atom);
	}
	return 0;
}

/* Writes a pyatom's data to an XML fragment in-memory */
void pyatom_innerxml(PyDictObject* atom, char* str, size_t maxlen){
	size_t writepos=0;
	
}

int
mpbook_from_pybook(MP_Book_c *mpbook, BookObject* pybook, MP_Dict_c* dict)
{
	// Given an mpbook already "create"d, this fills it in

	unsigned long int pynatoms = PyList_Size(pybook->atoms);
	if(pynatoms == 0){
		PyErr_SetString(PyExc_RuntimeError, "Attempted to load mpbook data from a pybook which is empty.\n");
		return 4;
	}
	if(mpbook->numAtoms != 0){
		// TODO LATER: could use mpbook->reset()
		PyErr_SetString(PyExc_RuntimeError, "Attempted to load data from a pybook into an mpbook which is not empty.\n");
		return 3;
	}


	mpbook->numChans   = pybook->numChans;
	mpbook->numSamples = pybook->numSamples;
	mpbook->sampleRate = pybook->sampleRate;

	for(unsigned long int i=0; i < pynatoms; ++i){
		// read atom from pybook, create one in mpbook
		PyObject* obj = PyList_GetItem(pybook->atoms, (Py_ssize_t)i);
		if(!PyDict_Check(obj)){
			PyErr_SetString(PyExc_RuntimeError, "Error -- iterating atoms in book, found entry is not a dict\n");
			return 1;
		}
		PyDictObject* pyatom = (PyDictObject*)obj;

		MP_Atom_c* mpatom = mpatom_from_pyatom(pyatom, mpbook->numChans, dict);

		if ( NULL==mpatom ) {
			delete mpbook;
			PyErr_SetString(PyExc_RuntimeError, "mpatom_from_pyatom() returned NULL while adding an atom to book\n");
			//return(NULL);
			return 5;
		} else {
			if(mpbook->append( mpatom ) != 1){
				PyErr_SetString(PyExc_RuntimeError, "Attempted to load too many atoms.\n");
				printf("Appending an atom from a pybook into an mpbook failed, probably due to out-of-memory issues. (py has %lu, mp has max of %lu.)\n",
						pynatoms, mpbook->maxNumAtoms);
				return 2;
			}
		}
	}

	//mpbook->recheck_num_samples();
	//mpbook->recheck_num_channels();

	return 0;
}

PyObject *
book_short_info(BookObject* pybook)
{
	pybook->mpbook->short_info();
	return Py_BuildValue("i", 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// __getstate__ and __setstate__ are to make it possible for python to do copy.deepcopy() or pickle.*()
// These two methods must be perfectly complementary.
// We simply implement by serialising elements in alphabetical order into a tuple:
//    ('atoms', 'numChans', 'numSamples', 'sampleRate')

PyObject * book_getstate(BookObject* self){
	return Py_BuildValue("Oiii", self->atoms, self->numChans, self->numSamples, self->sampleRate);
}

PyObject * book_setstate(BookObject* self, PyObject *args){
	PyObject *atoms;
	int numChans;
	int numSamples;
	int sampleRate;
	if (!PyArg_ParseTuple(args, "(Oiii)", &atoms, &numChans, &numSamples, &sampleRate))
		return NULL;

	PyObject* methodname = PyString_FromString("extend");
	PyObject* result = PyObject_CallMethodObjArgs(self->atoms, methodname, atoms, NULL);
	if(result != NULL){
		Py_DECREF(result);
	}
	Py_DECREF(methodname);

	self->numChans = numChans;
	self->numSamples = numSamples;
	self->sampleRate = sampleRate;
	Py_RETURN_NONE;
}

