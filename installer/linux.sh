#!/bin/bash

PLATFORM_WORD=`uname -m`
PLATFORM_NAME=`uname`
SEEDSDK_PATH="/media/d/SDK"
MYID=`id -u`

TOOL_PKG="aptitude"
TOOL_GCC="g++"
TOOL_GIT="git"

PACKAGES_GCC="gcc"
PACKAGES_GIT="git"
PACKAGES_SDL="libsdl-dev libsdl-image1.2-dev"

PROG_SDL=$(cat <<EOF
#include <SDL/SDL.h>

int main(int argc, char **argv)
{
	SDL_Init(0);
	return 1;
}
EOF)

echo "Welcome to the Seed Framework $PLATFORM_NAME installer!"

#if [[ $MYID -ne 0 ]]; then
#	echo "Not root."
#fi

function InstallPackages()
{
	echo "- Trying to install $* package(s), please enter the root password if needed."
	sudo $TOOL_PKG install $*
}

function CheckTools()
{
	echo "1. Checking tools..."
	type -P $TOOL_PKG &> /dev/null || { echo "$TOOL_PKG not found, is your distro supported? Please make a contribution and update this script!"; exit 1; }
	type -P $TOOL_GCC &> /dev/null || { echo "$TOOL_GCC not found."; InstallPackages $PACKAGES_GCC; } 
	type -P $TOOL_GIT &> /dev/null || { echo "$TOOL_GIT not found."; InstallPackages $PACKAGES_GIT; }
}

function CheckDependency()
{
	echo "2. Checking dependency..."
	echo $PROG_SDL > /tmp/sftest.cpp
	$TOOL_GCC `sdl-config --cflags` /tmp/sftest.cpp -o a.out `sdl-config --libs` &> /dev/null || { echo "SDL Library not found!"; InstallPackages $PACKAGES_SDL; }
}

function InstallDependency()
{
	echo "3. Installing dependency..."
}

function InstallFramework()
{
	echo "4. Installing Seed Framework..."
}

function SetupEnvironment()
{
	echo "5. Environment setup..."
	echo "" >> ~/.bashrc
	echo "# Seed Framework" >> ~/.bashrc
	echo "export SEEDSDK=\"$SEEDSDK_PATH\"" >> ~/.bashrc
}

function Build()
{
	echo "6. Building Seed Framework..."
}

#
# 1. Test if there is git, libogg, libvorbis, liboggfile, liboggplay, SDL, OpenAL, OpenGL, ...
CheckTools
CheckDependency
InstallDependency
# 2. clone SDK repository
InstallFramework
# 3. setup $SEEDSDK
#SetupEnvironment
# 4. build
Build
