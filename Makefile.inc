EigenInclude		:= 

LibMeshInclude		:= /usr/local/include/libmesh
LibMeshDir			:= /usr/local/lib

JacobiPolInclude	:= /home/soblinsky/MySoftware/jacobi_polynomial
JacobiPolDir		:= /home/soblinsky/MySoftware/jacobi_polynomial
JacobiPolLibFile	:= $(JacobiPolDir)/jacobi_polynomial.o

SandiaRuleInclude	:= /home/soblinsky/MySoftware/sandia_rules
SandiaRuleDir		:= /home/soblinsky/MySoftware/sandia_rules
SandiaRuleLibFile	:= $(SandiaRuleDir)/sandia_rules.o

GTestInclude		:= /usr/include
GTestDir				:= /usr/lib

GetPotInclude		:= /home/soblinsky/MySoftware/getpot-c++

MuParserInclude	:= /usr/local/include
MuParserDir			:= /usr/local/lib

#with BANANA I have segfault
#with DEPRECATED i never use the dlopen
#with VERBOSE I print information of the software flow
#with DESTRUCTOR_ALERT I print to the output everytime a destructor is called

GlobalDefines		:= \
							VERBOSE \
							DESTRUCTOR_ALERT \
							MYDEBUG \
							#BANANA \
							#DEPRECATED

