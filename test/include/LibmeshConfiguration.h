#ifndef __LIBMESH_CONFIGURATION_H
#define __LIBMESH_CONFIGURATION_H

#include "DinamicLoadConfiguration.h"

#include "libmesh/libmesh.h" //libMesh::LibMeshInit

//As for LogFile class, this one redirects libMesh buffers on file
class LibmeshLogFile
{
	public:
		LibmeshLogFile(const std::string&);
		~LibmeshLogFile();

	private:
		std::ofstream _ofs;
		std::basic_streambuf<char>* _libmesh_err_buf;
		std::basic_streambuf<char>* _libmesh_out_buf;
};

//Configuration used by tests which need libMesh framework
class LibmeshTest : public LoadTest
{
	protected:
		LibmeshTest();
		virtual ~LibmeshTest();
		virtual void SetUp()override;
		virtual void TearDown()override;

		LibmeshLogFile _libmesh_out;
		std::unique_ptr<libMesh::LibMeshInit> _mesh_init_ptr;		
};

#endif //__LIBMESH_CONFIGURATION_H
