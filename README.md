This is a preview release of GraphObjects, a new graph persistence framework targeeting MacOS X and iOS. As such it is incomplete and has a number of issues.

__Support__

GraphObjects is intended to work on Mac OS X 10.6+ and iOS 4.2+. The underlying C++ based implementations should be portable to other platforms, and it is an eventual requirement that they work on Linux. The Objective C interfaces depend on features of the modern runtime and blocks, so they will not work on older Mac OS X or iOS releases.

__Building__

GraphObjects requires a clang 2.0 toolchain in order to build. that mean that no currently released build of Xcode can build it out of box. In order to build it you either need to build a a custome MacOS X 10.6 or iOS 4.2 SDK a copy of clang pulled from their repository, hack up your system compilers, or use unreleased developer tool previews. While GraphObjects has occasionally been built for iOS, it currently is being primarily brought up on Mac OS X x86.
    
__Usage__

You probably don't want to use this yet unless you are intending to hack on it. It's API is similiar to CoreData, though there is no modeler (it derives object info directly from LGGOObjects). The only header files that should be considered public to framework clients are LGGOGraph.h and LGGOGraphContext.h, and even those currently leak some private details.

Mailing List: [http://groups.google.com/group/graphobjects](http://groups.google.com/group/graphobjects)