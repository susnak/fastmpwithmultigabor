from distutils.core import setup,Extension

src = ['pyMPTK.cpp', 'pyMPTK_book.cpp', 'pyMPTK_decompose.cpp', 'pyMPTK_atom.cpp']

py_modules = ['mptkplot']

inc_dir = ['/usr/include', '@MPTK_SOURCE_DIR@','@MPTK_SOURCE_DIR@/src/utils/libtinyxml', '@MPTK_SOURCE_DIR@/src/libmptk/', '@MPTK_BINARY_DIR@' , '@MPTK_BINARY_DIR@/src/libmptk/', '@LIBDSP_INCLUDE_DIR@','@FFTW3_INCLUDE_DIR@', '@SNDFILE_INCLUDE_DIR@', '@NUMPY_INCLUDE_DIRS@']


lib =['@MPTK_LIB_DIR@/@LIB_MPTK_OUTPUT_NAME@','@MPTK_LIB_DIR@/dsp_windows']

setup (name = "mptk",
	version = "0.1",
	description = "Python bindings for Matching Pursuit ToolKit",
	author = "Emmanuel Ravelli and Dan Stowell",
	author_email = "ravelli@lam.jussieu.fr",
	ext_modules = [Extension('mptk', src, include_dirs=inc_dir, libraries=lib)],
	py_modules = py_modules,
)

