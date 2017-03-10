#!/bin/sh
clear ;make;
cd ./example;
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:../plugin_loading/lib;
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:../interpolating_functions/lib;
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:../quadrature_rules/libmesh_quadrature/lib;
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:../library_bridges/libmesh_bridge/lib;
./bin/example_Debug
cd ..
