set(classes
  vtkODBCDatabase
  vtkODBCQuery)

vtk_module_add_module(VTK::IOODBC
  CLASSES ${classes}
  SOURCES vtkIOODBC_AutoInit.cxx)

vtk_module_find_package(PRIVATE_IF_SHARED
  PACKAGE ODBC)
vtk_module_link(VTK::IOODBC
  PRIVATE
    ODBC::ODBC)
vtk_add_test_mangling(VTK::IOODBC)
