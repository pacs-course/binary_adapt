###############################################################################
#	external libraries installation paths													#
#	this file will be included by software components to get the position of	#
#	the libraries they depend on.																#
#	Put in ***Include environment variable the path to library headers,			#
#	in ***Dir environment variable the path to library file.							#
###############################################################################

#	path to Eigen directory
#	include <Eigen/Dense> will be called in the software
EigenInclude		:= /usr/local/include/eigen3

LibMeshInclude		:= /usr/local/include/libmesh
LibMeshDir			:= /usr/local/lib

GTestInclude		:= /usr/include
GTestDir				:= /usr/lib

#until libMesh is not patched libMesh getpot has to be used 
GetPotInclude		:= $(LibMeshInclude)
#GetPotInclude		:= /usr/local/include

MuParserInclude	:= /usr/local/include
MuParserDir			:= /usr/local/lib

##########################################################
#	libraries released as sources.								#
#	Put in:																#
#		***Include variable the path to the headers;			#
#		***Dir variable the path to the sources;				#
#		***LibFile variable the name of a linkable file		#
#	If the ***LibFile does not exist, the first Makefile	#
#	that need the library will provide to its creation.	#
#	DO NOT leave it blank!											#
##########################################################

JacobiPolInclude	:= $(BaseDir)/jacobi_polynomial
JacobiPolSrcDir	:= $(BaseDir)/jacobi_polynomial
JacobiPolLibFile	:= $(JacobiPolSrcDir)/jacobi_polynomial.o

SandiaRuleInclude	:= $(BaseDir)/sandia_rules
SandiaRuleSrcDir	:= $(BaseDir)/sandia_rules
SandiaRuleLibFile	:= $(SandiaRuleSrcDir)/sandia_rules.o


#	macros to be defined in every Makefile
#	with VERBOSE defined informations on the software flow will be printed

GlobalDefines		:= \
							VERBOSE \
