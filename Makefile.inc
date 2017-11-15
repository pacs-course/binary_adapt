EigenInclude		:= /usr/local/include

LibMeshInclude		:= /usr/local/include/libmesh
LibMeshDir			:= /usr/local/lib

JacobiPolInclude	:= $(HOME)/MySoftware/jacobi_polynomial
JacobiPolDir		:= $(HOME)/MySoftware/jacobi_polynomial
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
#with DEPRECATED i use deprecated sections of the code
#with VERBOSE I print information of the software flow
#with DESTRUCTOR_ALERT I print to the output everytime a destructor is called

GlobalDefines		:= \
							VERBOSE \
							#MYDEBUG \
							#DESTRUCTOR_ALERT \
							#BANANA \
							#DEPRECATED

