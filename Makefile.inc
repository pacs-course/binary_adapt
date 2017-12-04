#-----------------------------------------------------------------------------#
#	external libraries installation paths													#
#	this file will be included by software components to get the position of	#
#	the libraries they depend on.																#
#	Put in ***Include environment variable the path to library headers,			#
#	in ***Dir environment variable the path to library file.							#
#-----------------------------------------------------------------------------#

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

#--------------------------------------------------------#
#	libraries released as sources.								#
#	Put in:																#
#		***Include	variable the path to the headers;		#
#		*******Dir	variable the path to the sources;		#
#		***LibFile	variable the name of a linkable file.	#
#																			#
#	If the ***LibFile environment variable is left blank,	#
#	every Makefile that needs the file will provide			#
#	to create its local copy from sources.						#
#--------------------------------------------------------#

JacobiPolInclude	:= $(BaseDir)/jacobi_polynomial
JacobiPolSrcDir	:= $(BaseDir)/jacobi_polynomial
#JacobiPolLibFile	:= $(JacobiPolSrcDir)/jacobi_polynomial.o

SandiaRuleInclude	:= $(BaseDir)/sandia_rules
SandiaRuleSrcDir	:= $(BaseDir)/sandia_rules
#SandiaRuleLibFile:= $(SandiaRuleSrcDir)/sandia_rules.o


#--------------------------------------------------#
#	The path where the libraries will be installed	#
#--------------------------------------------------#

InstallDir	= $(BaseDir)/lib

#----------------------!!!!NOTE!!!!----------------------#
#																			#
# BaseDir variable content will be recognized				#
# by Makefiles including it as path to root directory		#
# [the one where this file is stored in]						#
# DO NOT edit its content to ensure correct behaviour		#
#																			#
#--------------------------------------------------------#


#-----------------------------------------------------------#
#	macros to be defined in every Makefile.						#
#	Define:																	#
#																				#
#		- VERBOSE to print informations on the software flow	#
#		- LIBMESH_BUG_FIXED if using the patched libMesh and	#
#		  want to link GetPot instead of libMesh version one	#
#		- INTERNAL_LEGENDRE to use my implementation of			#
#		  Legendre polynomials instead of jacobi_polynomial	#
#																				#
#-----------------------------------------------------------#

GlobalDefines		:= \
							VERBOSE \

