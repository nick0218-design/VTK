vtk_module_test_data(
  Data/AMR/htg3d.htg
  Data/HTG/donut_XZ_shift_2d.htg
  Data/HTG/shell_3d.htg
  Data/HTG/single_cell_3d.htg
  Data/HTG/three_cells_3d.htg)

if (NOT vtk_testing_cxx_disabled)
  add_subdirectory(Cxx)
endif ()

if (VTK_WRAP_PYTHON)
  add_subdirectory(Python)
endif ()
