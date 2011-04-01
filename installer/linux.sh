#!/bin/bash

PLATFORM_WORD=`uname -m`
PLATFORM_NAME=`uname`
SEEDSDK_PATH="/media/d/SDK"
MYID=`id -u`

TOOL_PKG="aptitude"
TOOL_GCC="gcc"
TOOL_GIT="git"

echo "Welcome to the Seed Framework $PLATFORM_NAME installer!"

#if [[ $MYID -ne 0 ]]; then
#	echo "Not root."
#fi

function InstallPackage()
{
	echo "- Trying to install $1 package, please enter the root password if needed."
	sudo $TOOL_APT install $1
}

function CheckTools()
{
	echo "1. Checking tools..."
	type -P $TOOL_PKG &>/dev/null || { echo "$TOOL_PKG not found, is your distro supported? Please make a contribution and update this script!"; exit 1; }
	type -P $TOOL_GCC &>/dev/null || { echo "$TOOL_GCC not found."; InstallPackage gcc; } 
	type -P $TOOL_GIT &>/dev/null || { echo "$TOOL_GIT not found."; InstallPackage git; }
}

function CheckDependency()
{
	echo "2. Checking dependency..."
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
