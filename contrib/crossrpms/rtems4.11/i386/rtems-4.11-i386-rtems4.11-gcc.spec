#
# Please send bugfixes or comments to
# 	http://www.rtems.org/bugzilla
#

%define _prefix                 /opt/rtems-4.11
%define _exec_prefix            %{_prefix}
%define _bindir                 %{_exec_prefix}/bin
%define _sbindir                %{_exec_prefix}/sbin
%define _libexecdir             %{_exec_prefix}/libexec
%define _datarootdir            %{_prefix}/share
%define _datadir                %{_datarootdir}
%define _sysconfdir             %{_prefix}/etc
%define _sharedstatedir         %{_prefix}/com
%define _localstatedir          %{_prefix}/var
%define _includedir             %{_prefix}/include
%define _libdir                 %{_exec_prefix}/%{_lib}
%define _mandir                 %{_datarootdir}/man
%define _infodir                %{_datarootdir}/info
%define _localedir              %{_datarootdir}/locale

%ifos cygwin cygwin32 mingw mingw32
%define _exeext .exe
%define debug_package           %{nil}
%define _libdir                 %{_exec_prefix}/lib
%else
%define _exeext %{nil}
%endif

%ifos cygwin cygwin32
%define optflags -O3 -pipe -march=i486 -funroll-loops
%endif

%ifos mingw mingw32
%if %{defined _mingw32_cflags}
%define optflags %{_mingw32_cflags}
%else
%define optflags -O2 -g -pipe -Wall -Wp,-D_FORTIFY_SOURCE=2 -fexceptions --param=ssp-buffer-size=4 -mms-bitfields
%endif
%endif

%if "%{_build}" != "%{_host}"
%define _host_rpmprefix %{_host}-
%else
%define _host_rpmprefix %{nil}
%endif


%define gcc_pkgvers 4.6.2
%define gcc_version 4.6.2
%define gcc_rpmvers %{expand:%(echo "4.6.2" | tr - _ )}

%define newlib_pkgvers		1.19.0
%define newlib_version		1.19.0

Name:         	rtems-4.11-i386-rtems4.11-gcc
Summary:      	i386-rtems4.11 gcc

Group:	      	Development/Tools
Version:        %{gcc_rpmvers}
Release:      	4%{?dist}
License:      	GPL
URL:		http://gcc.gnu.org
BuildRoot:	%{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

%define _use_internal_dependency_generator 0

BuildRequires:  %{_host_rpmprefix}gcc

# FIXME: Disable lto for now, to avoid dependencies on libelf
%bcond_with lto

# FIXME: Disable python gdb scripts
# ATM, no idea how to package them
%bcond_with pygdb

# FIXME: Disable GCC-plugin
# Bug in gcc-4.5-20100318, doesn't build them on x86_84 hosts.
%bcond_with plugin

# EXPERIMENTAL: Use gcc's stdint.h instead of newlib's
# Should be applicable to gcc >= 4.5.0
%bcond_with gcc_stdint

# EXPERIMENTAL: Enable newlib's iconv
%bcond_without iconv

# versions of libraries, we conditionally bundle if necessary
%global mpc_version	0.8.1
%global mpfr_version	2.4.2
%global gmp_version	4.3.2
%global libelf_version  0.8.13

# versions of libraries these distros are known to ship
%if 0%{?fc16}
%global mpc_provided 0.8.3
%global mpfr_provided 3.0.0
%global gmp_provided 4.3.2
%endif

%if 0%{?fc15}
%global mpc_provided 0.8.3
%global mpfr_provided 3.0.0
%global gmp_provided 4.3.2
%endif

%if 0%{?fc14}
%global mpc_provided 0.8.1
%global mpfr_provided 2.4.2
%global gmp_provided 4.3.1
%endif

%if 0%{?el6}
%global mpc_provided %{nil}
%global mpfr_provided 2.4.1
%global gmp_provided 4.3.1
%endif

%if 0%{?el5}
%global mpc_provided %{nil}
%global mpfr_provided %{nil}
%global gmp_provided 4.1.4
%endif

%if 0%{?suse12_1}
%global mpc_provided 0.8.2
%global mpfr_provided 3.0.1
%global gmp_provided 5.0.2
%endif

%if 0%{?suse11_3}
%global mpc_provided 0.8.1
%global mpfr_provided 2.4.2
%global gmp_provided 4.3.2
%endif

%if 0%{?suse11_4}
%global mpc_provided 0.8.2
%global mpfr_provided 3.0.0
%global gmp_provided 5.0.1
%endif

%if 0%{?cygwin}
%global mpc_provided 0.8
%global mpfr_provided 2.4.1
%global gmp_provided 4.3.1
%endif

%if 0%{?mingw32}
%global mpc_provided 0.8.1
%global mpfr_provided 2.4.1
%global gmp_provided 4.3.2
%endif

%if "%{gcc_version}" >= "4.2.0"
%endif

%if "%{gcc_version}" >= "4.3.0"
%define gmp_required		4.1
%define mpfr_required		2.3.1
%endif

%if "%{gcc_version}" >= "4.3.3"
%define cloog_required 		0.15
%endif

%if "%{gcc_version}" >= "4.4.0"
%define mpfr_required		2.3.2
%endif

%if "%{gcc_version}" >= "4.5.0"
%define mpc_required 		0.8
%if %{with lto}
%define libelf_required 	0.8.12
%endif
%endif

%if %{defined mpc_required}
%if "%{mpc_provided}" >= "%{mpc_required}"
%{?fedora:BuildRequires: libmpc-devel >= %{mpc_required}}
%{?suse:BuildRequires: mpc-devel >= %{mpc_required}}
%if "%{_build}" != "%{_host}"
BuildRequires:  %{_host_rpmprefix}mpc-devel >= %{mpc_required}
%endif
%else
%define _build_mpc 1
%define gmp_required 		4.2
%endif
%endif

%if %{defined gmp_required}
%if "%{gmp_provided}" >= "%{gmp_required}"
BuildRequires: gmp-devel >= %{gmp_required}
%if "%{_build}" != "%{_host}"
BuildRequires:  %{_host_rpmprefix}gmp-devel >= %{gmp_required}
%endif
%else
%define _build_gmp 1
%endif
%endif

%if %{defined libelf_required}
%if "%{libelf_provided}" >= "%{libelf_required}"
BuildRequires: libelf-devel >= %{libelf_required}
%if "%{_build}" != "%{_host}"
BuildRequires:  %{_host_rpmprefix}libelf-devel >= %{libelf_required}
%endif
%else
%define _build_libelf 1
%endif
%endif


%if %{defined cloog_required}
%{?fc14:BuildRequires: cloog-ppl-devel >= %cloog_required}
%{?fc15:BuildRequires: cloog-ppl-devel >= %cloog_required}
%{?fc16:BuildRequires: cloog-ppl-devel >= %cloog_required}
%{?el6:BuildRequires: cloog-ppl-devel >= %cloog_required}
%{?suse12_1:BuildRequires: cloog-devel >= %cloog_required, ppl-devel}
%{?suse11_4:BuildRequires: cloog-devel >= %cloog_required, ppl-devel}
%{?suse11_3:BuildRequires: cloog-devel >= %cloog_required, ppl-devel}
%endif


%if %{defined mpfr_required}
%if "%{mpfr_provided}" >= "%{mpfr_required}"
BuildRequires: mpfr-devel >= %{mpfr_required}
%if "%{_build}" != "%{_host}"
BuildRequires:  %{_host_rpmprefix}mpfr-devel >= %{mpfr_required}
%endif
%else
%define _build_mpfr 1
%endif
%endif

%if "%{_build}" != "%{_host}"
BuildRequires:  rtems-4.11-i386-rtems4.11-gcc = %{gcc_rpmvers}
%endif

# Not strictly required, but patches may introduce a need to use them.
# For reasons of simplicity, always require them.
BuildRequires:	flex bison

BuildRequires:	texinfo >= 4.2
BuildRequires:	rtems-4.11-i386-rtems4.11-binutils

Requires:	rtems-4.11-gcc-common
Requires:	rtems-4.11-i386-rtems4.11-binutils
Requires:	rtems-4.11-i386-rtems4.11-gcc-libgcc = %{gcc_rpmvers}-%{release}
Requires:	rtems-4.11-i386-rtems4.11-newlib = %{newlib_version}-23%{?dist}

%if "%{gcc_version}" >= "4.5.0"
BuildRequires:  zlib-devel
%if "%{_build}" != "%{_host}"
BuildRequires:  %{_host_rpmprefix}zlib-devel
%endif
%else
%endif

%global _gcclibdir %{_prefix}/lib

%if "%{gcc_version}" == "4.6.2"
Source0:	ftp://ftp.gnu.org/gnu/gcc/gcc-%{gcc_pkgvers}/gcc-core-%{gcc_pkgvers}.tar.bz2
Patch0:         ftp://ftp.rtems.org/pub/rtems/SOURCES/4.11/gcc-core-4.6.2-rtems4.11-20111124.diff
%endif
%if "%{gcc_version}" == "4.5.3"
Source0:	ftp://ftp.gnu.org/gnu/gcc/gcc-%{gcc_pkgvers}/gcc-core-%{gcc_pkgvers}.tar.bz2
Patch0:         ftp://ftp.rtems.org/pub/rtems/SOURCES/4.11/gcc-core-4.5.3-rtems4.11-20110905.diff
%endif

%if "%{gcc_version}" == "4.6.2"
Source1:	ftp://ftp.gnu.org/gnu/gcc/gcc-%{gcc_pkgvers}/%{gcc_pkgvers}/gcc-g++-%{gcc_pkgvers}.tar.bz2
%endif
%if "%{gcc_version}" == "4.5.3" 
Source1:	ftp://ftp.gnu.org/gnu/gcc/gcc-%{gcc_pkgvers}/gcc-g++-%{gcc_pkgvers}.tar.bz2
Patch1:		ftp://ftp.rtems.org/pub/rtems/SOURCES/4.11/gcc-g++-4.5.3-rtems4.11-20110905.diff
%endif

%if "%{newlib_version}" == "1.19.0"
Source50:	ftp://sources.redhat.com/pub/newlib/newlib-%{newlib_pkgvers}.tar.gz
Patch50:	ftp://ftp.rtems.org/pub/rtems/SOURCES/4.11/newlib-1.19.0-rtems4.11-20111006.diff
%endif

%if 0%{?_build_mpfr}
Source60:    http://www.mpfr.org/mpfr-%{mpfr_version}/mpfr-%{mpfr_version}.tar.bz2
%endif

%if 0%{?_build_mpc}
Source61:    http://www.multiprecision.org/mpc/download/mpc-%{mpc_version}.tar.gz
%endif

%if 0%{?_build_gmp}
Source62:    ftp://ftp.gnu.org/gnu/gmp/gmp-%{gmp_version}.tar.bz2
%endif

%if 0%{?_build_libelf}
Source63:    http://www.mr511.de/software/libelf-%{libelf_version}.tar.gz
%endif

%description
Cross gcc for i386-rtems4.11.

%prep
%setup -c -T -n %{name}-%{version}

%setup -q -T -D -n %{name}-%{version} -a0
cd gcc-%{gcc_pkgvers}
%{?PATCH0:%patch0 -p1}
cd ..

%{?SOURCE1:%setup -q -T -D -n %{name}-%{version} -a1}
cd gcc-%{gcc_pkgvers}
%{?PATCH1:%patch1 -p1}
cd ..





%if %{with gcc_stdint}
sed -i -e '/thread_file=.*rtems/,/use_gcc_stdint=wrap/ { s/use_gcc_stdint=wrap/use_gcc_stdint=provide/}' gcc-%{gcc_pkgvers}/gcc/config.gcc
%endif

%setup -q -T -D -n %{name}-%{version} -a50
cd newlib-%{newlib_version}
%{?PATCH50:%patch50 -p1}
cd ..
  # Copy the C library into gcc's source tree
  ln -s ../newlib-%{newlib_version}/newlib gcc-%{gcc_pkgvers}
%if %{with gcc_stdint}
  rm gcc-%{gcc_pkgvers}/newlib/libc/include/stdint.h
%endif
  # Make sure not to be using GPL'ed sources
  rm -rf gcc-%{gcc_pkgvers}/newlib/libc/sys/linux
  rm -rf gcc-%{gcc_pkgvers}/newlib/libc/sys/rdos

%if 0%{?_build_mpfr}
%setup -q -T -D -n %{name}-%{version} -a60
%{?PATCH60:%patch60 -p1}
  # Build mpfr one-tree style
  ln -s ../mpfr-%{mpfr_version} gcc-%{gcc_pkgvers}/mpfr
%endif

%if 0%{?_build_mpc}
%setup -q -T -D -n %{name}-%{version} -a61
%{?PATCH61:%patch61 -p1}
  # Build mpc one-tree style
  ln -s ../mpc-%{mpc_version} gcc-%{gcc_pkgvers}/mpc
%endif

%if 0%{?_build_gmp}
%setup -q -T -D -n %{name}-%{version} -a62
%{?PATCH62:%patch62 -p1}
  # Build gmp one-tree style
  ln -s ../gmp-%{gmp_version} gcc-%{gcc_pkgvers}/gmp
%endif

%if 0%{?_build_libelf}
%setup -q -T -D -n %{name}-%{version} -a63
%{?PATCH63:%patch63 -p1}
  # Build libelf one-tree style
  ln -s ../libelf-%{libelf_version} gcc-%{gcc_pkgvers}/libelf
%endif

echo "RTEMS gcc-%{gcc_version}-4%{?dist}/newlib-%{newlib_version}-23%{?dist}" > gcc-%{gcc_pkgvers}/gcc/DEV-PHASE


  # Fix timestamps
  cd gcc-%{gcc_pkgvers}
  contrib/gcc_update --touch
  cd ..
%build
  mkdir -p build

  cd build

  languages="c"
  languages="$languages,c++"
  export PATH="%{_bindir}:${PATH}"
%if "%{_build}" != "%{_host}"
  CFLAGS_FOR_BUILD="-g -O2 -Wall" \
  CC="%{_host}-gcc ${RPM_OPT_FLAGS}" \
%else
# gcc is not ready to be compiled with -std=gnu99
  CC=$(echo "%{__cc} ${RPM_OPT_FLAGS}" | sed -e 's,-std=gnu99 ,,') \
%endif
  ../gcc-%{gcc_pkgvers}/configure \
    --prefix=%{_prefix} \
    --bindir=%{_bindir} \
    --exec_prefix=%{_exec_prefix} \
    --includedir=%{_includedir} \
    --libdir=%{_gcclibdir} \
    --libexecdir=%{_libexecdir} \
    --mandir=%{_mandir} \
    --infodir=%{_infodir} \
    --datadir=%{_datadir} \
    --build=%_build --host=%_host \
    --target=i386-rtems4.11 \
    --disable-libstdcxx-pch \
    --with-gnu-as --with-gnu-ld --verbose \
    --with-newlib \
    --with-system-zlib \
    --disable-nls --without-included-gettext \
    --disable-win32-registry \
    --enable-version-specific-runtime-libs \
    --enable-threads \
    %{?with_lto:--enable-lto}%{!?with_lto:--disable-lto} \
    %{?with_plugin:--enable-plugin}%{!?with_plugin:--disable-plugin} \
    --enable-newlib-io-c99-formats \
    %{?with_iconv:--enable-newlib-iconv} \
    --enable-languages="$languages"

%if "%_host" != "%_build"
  # Bug in gcc-3.2.1:
  # Somehow, gcc doesn't get syslimits.h right for Cdn-Xs
  mkdir -p gcc/include
  cp ../gcc-%{gcc_pkgvers}/gcc/gsyslimits.h gcc/include/syslimits.h
%endif

  make %{?_smp_mflags} all
  make info
  cd ..

%install
  export PATH="%{_bindir}:${PATH}"
  rm -rf $RPM_BUILD_ROOT

  cd build

  make DESTDIR=$RPM_BUILD_ROOT install
  cd ..

  cd build/i386-rtems4.11/newlib
  make DESTDIR=$RPM_BUILD_ROOT install-info
  cd ../../..

%if "%{gcc_version}" <= "4.1.2"
# Misplaced header file
  if test -f $RPM_BUILD_ROOT%{_includedir}/mf-runtime.h; then
    mv $RPM_BUILD_ROOT%{_includedir}/mf-runtime.h \
      $RPM_BUILD_ROOT%{_gcclibdir}/gcc/i386-rtems4.11/%{gcc_version}/include/
  fi
%endif

  # host library
%if "%{gcc_version}" >= "4.2.0"
  # libiberty doesn't honor --libdir, but always installs to a 
  # magically guessed _libdir
  rm -f  ${RPM_BUILD_ROOT}%{_libdir}/libiberty.a
%else
  # libiberty installs to --libdir=...
  rm -f ${RPM_BUILD_ROOT}%{_gcclibdir}/libiberty.a
%endif

  # We use the version from binutils
  rm -f $RPM_BUILD_ROOT%{_bindir}/i386-rtems4.11-c++filt%{_exeext}


  # We don't ship info/dir
  rm -f $RPM_BUILD_ROOT%{_infodir}/dir
  touch $RPM_BUILD_ROOT%{_infodir}/dir

  # We don't want libffi's man-pages
  rm -f $RPM_BUILD_ROOT%{_mandir}/man3/*ffi*

  # Bug in gcc-3.4.0pre
  rm -f $RPM_BUILD_ROOT%{_bindir}/i386-rtems4.11-i386-rtems4.11-gcjh%{_exeext}

  # Bug in gcc-3.3.x/gcc-3.4.x: Despite we don't need fixincludes, it installs
  # the fixinclude-install-tools
  rm -rf ${RPM_BUILD_ROOT}%{_gcclibdir}/gcc/i386-rtems4.11/%{gcc_version}/install-tools
  rm -rf ${RPM_BUILD_ROOT}%{_libexecdir}/gcc/i386-rtems4.11/%{gcc_version}/install-tools

  # Bug in gcc > 4.1.0: Installs an unused, empty directory
  if test -d ${RPM_BUILD_ROOT}%{_prefix}/i386-rtems4.11/include/bits; then
    rmdir ${RPM_BUILD_ROOT}%{_prefix}/i386-rtems4.11/include/bits
  fi

%if %{with iconv}
  # Iconv enabled newlib installs external ccts (target files)
  if test -d ${RPM_BUILD_ROOT}%{_datadir}/iconv_data; then
    rm -rf ${RPM_BUILD_ROOT}%{_datadir}/iconv_data
  fi
%endif

%if ! %{with pygdb}
  # gcc >= 4.5.0: installs weird libstdc++ python bindings.
  if test -d ${RPM_BUILD_ROOT}%{_datadir}/gcc-%{gcc_version}/python; then
    rm -rf ${RPM_BUILD_ROOT}%{_datadir}/gcc-%{gcc_version}/python/libstdcxx
  fi

%endif
  # Collect multilib subdirectories
  multilibs=`build/gcc/xgcc -Bbuild/gcc/ --print-multi-lib | sed -e 's,;.*$,,'`

  echo "%defattr(-,root,root,-)" > build/files.newlib
  TGTDIR="%{_exec_prefix}/i386-rtems4.11/lib"
  for i in $multilibs; do
    case $i in
    \.) echo "%dir ${TGTDIR}" >> build/files.newlib
      ;;
    *)  echo "%dir ${TGTDIR}/$i" >> build/files.newlib
      ;;
    esac
  done

  rm -f dirs ;
  echo "%defattr(-,root,root,-)" >> dirs
  TGTDIR="%{_gcclibdir}/gcc/i386-rtems4.11/%{gcc_version}"
  for i in $multilibs; do
    case $i in
    \.) ;; # ignore, handled elsewhere
    *)  echo "%dir ${TGTDIR}/$i" >> dirs
      ;;
    esac
  done

  # Collect files to go into different packages
  cp dirs build/files.gcc
  cp dirs build/files.gfortran
  cp dirs build/files.objc
  cp dirs build/files.gcj
  cp dirs build/files.g++
  cp dirs build/files.go

  TGTDIR="%{_gcclibdir}/gcc/i386-rtems4.11/%{gcc_version}"
  f=`find ${RPM_BUILD_ROOT}${TGTDIR} ! -type d -print | sed -e "s,^$RPM_BUILD_ROOT,,g"`;
  for i in $f; do
    case $i in
    *lib*.la) rm ${RPM_BUILD_ROOT}/$i ;; # ignore: gcc produces bogus libtool libs
    *f771) ;;
    *f951) ;;
    *cc1) ;;
    *cc1obj) ;;
    *cc1plus) ;; # ignore: explicitly put into rpm elsewhere
    *collect2) ;;
    *libobjc*) echo "$i" >> build/files.objc ;;
    *go1) ;; # ignore: explicitly put into rpm elsewhere
    *include/objc*) ;;
    *include/g++*);;
    *include/c++*);;
    *include-fixed/*);;
    *finclude/*);;
    */go/*) ;; # ignore : explicitly put into rpm elsewhere
    *adainclude*);;
    *adalib*);;
    *gnat1);;
    *jc1) ;;
    *jvgenmain) ;;
    */libgfortran*.*) echo "$i" >> build/files.gfortran ;;
    */libgo.*) echo "$i" >> build/files.go ;;
    %{!?with_pygdb:*/libstdc++*gdb.py*) rm ${RPM_BUILD_ROOT}/$i ;;} # ignore for now
    %{?with_pygdb:*/libstdc++*gdb.py*) >> build/files.g++ ;;}
    */libstdc++.*) echo "$i" >> build/files.g++ ;;
    */libsupc++.*) echo "$i" >> build/files.g++ ;;
    *) echo "$i" >> build/files.gcc ;;
    esac
  done

  TGTDIR="%{_exec_prefix}/i386-rtems4.11/lib"
  f=`find ${RPM_BUILD_ROOT}${TGTDIR} ! -type d -print | sed -e "s,^$RPM_BUILD_ROOT,,g"`;
  for i in $f; do
    case $i in
    *lib*.la) rm ${RPM_BUILD_ROOT}/$i;; # ignore - gcc produces bogus libtool libs
    *libiberty.a) rm ${RPM_BUILD_ROOT}/$i ;; # ignore - GPL'ed
# all other files belong to newlib
    *) echo "$i" >> build/files.newlib ;; 
    esac
  done
# Extract %%__os_install_post into os_install_post~
cat << \EOF > os_install_post~
%__os_install_post
EOF

# Generate customized brp-*scripts
cat os_install_post~ | while read a x y; do
case $a in
# Prevent brp-strip* from trying to handle foreign binaries
*/brp-strip*)
  b=$(basename $a)
  sed -e 's,find $RPM_BUILD_ROOT,find $RPM_BUILD_ROOT%_bindir $RPM_BUILD_ROOT%_libexecdir,' $a > $b
  chmod a+x $b
  ;;
# Fix up brp-compress to handle %%_prefix != /usr
*/brp-compress*)
  b=$(basename $a)
  sed -e 's,\./usr/,.%{_prefix}/,g' < $a > $b
  chmod a+x $b
  ;;
esac
done

sed -e 's,^[ ]*/usr/lib/rpm.*/brp-strip,./brp-strip,' \
  -e 's,^[ ]*/usr/lib/rpm.*/brp-compress,./brp-compress,' \
< os_install_post~ > os_install_post 
%define __os_install_post . ./os_install_post


cat << EOF > %{_builddir}/%{name}-%{gcc_rpmvers}/find-provides
#!/bin/sh
grep -E -v '^${RPM_BUILD_ROOT}%{_exec_prefix}/i386-rtems4.11/(lib|include|sys-root)' \
  %{?_gcclibdir:| grep -v '^${RPM_BUILD_ROOT}%{_gcclibdir}/gcc/i386-rtems4.11/'} | %__find_provides
EOF
chmod +x %{_builddir}/%{name}-%{gcc_rpmvers}/find-provides
%define __find_provides %{_builddir}/%{name}-%{gcc_rpmvers}/find-provides

cat << EOF > %{_builddir}/%{name}-%{gcc_rpmvers}/find-requires
#!/bin/sh
grep -E -v '^${RPM_BUILD_ROOT}%{_exec_prefix}/i386-rtems4.11/(lib|include|sys-root)' \
  %{?_gcclibdir:| grep -v '^${RPM_BUILD_ROOT}%{_gcclibdir}/gcc/i386-rtems4.11/'} | %__find_requires
EOF
chmod +x %{_builddir}/%{name}-%{gcc_rpmvers}/find-requires
%define __find_requires %{_builddir}/%{name}-%{gcc_rpmvers}/find-requires

%ifnarch noarch
# Extract %%__debug_install_post into debug_install_post~
cat << \EOF > debug_install_post~
%__debug_install_post
EOF

# Generate customized debug_install_post script
cat debug_install_post~ | while read a x y; do
case $a in
# Prevent find-debuginfo.sh* from trying to handle foreign binaries
*/find-debuginfo.sh)
  b=$(basename $a)
  sed -e 's,find "$RPM_BUILD_ROOT" !,find "$RPM_BUILD_ROOT"%_bindir "$RPM_BUILD_ROOT"%_libexecdir !,' $a > $b
  chmod a+x $b
  ;;
esac
done

sed -e 's,^[ ]*/usr/lib/rpm/find-debuginfo.sh,./find-debuginfo.sh,' \
< debug_install_post~ > debug_install_post 
%define __debug_install_post . ./debug_install_post

%endif

%clean
  rm -rf $RPM_BUILD_ROOT

# ==============================================================
# rtems-4.11-i386-rtems4.11-gcc
# ==============================================================
# %package -n rtems-4.11-i386-rtems4.11-gcc
# Summary:        GNU cc compiler for i386-rtems4.11
# Group:          Development/Tools
# Version:        %{gcc_rpmvers}
# Requires:       rtems-4.11-i386-rtems4.11-binutils
# Requires:       rtems-4.11-i386-rtems4.11-newlib = %{newlib_version}-23%{?dist}
# License:	GPL

# %if %build_infos
# Requires:      rtems-4.11-gcc-common
# %endif

%description -n rtems-4.11-i386-rtems4.11-gcc
GNU cc compiler for i386-rtems4.11.

# ==============================================================
# rtems-4.11-i386-rtems4.11-gcc-libgcc
# ==============================================================
%package -n rtems-4.11-i386-rtems4.11-gcc-libgcc
Summary:        libgcc for i386-rtems4.11-gcc
Group:          Development/Tools
Version:        %{gcc_rpmvers}
%{?_with_noarch_subpackages:BuildArch: noarch}
Requires:       rtems-4.11-i386-rtems4.11-newlib = %{newlib_version}-23%{?dist}
License:	GPL

%description -n rtems-4.11-i386-rtems4.11-gcc-libgcc
libgcc i386-rtems4.11-gcc.


%files -n rtems-4.11-i386-rtems4.11-gcc
%defattr(-,root,root)
%dir %{_prefix}

%dir %{_mandir}
%dir %{_mandir}/man1
%{_mandir}/man1/i386-rtems4.11-gcc.1*
%{_mandir}/man1/i386-rtems4.11-cpp.1*
%{_mandir}/man1/i386-rtems4.11-gcov.1*

%dir %{_bindir}
%{_bindir}/i386-rtems4.11-cpp%{_exeext}
%{_bindir}/i386-rtems4.11-gcc%{_exeext}
%{_bindir}/i386-rtems4.11-gcc-%{gcc_version}%{_exeext}
%{_bindir}/i386-rtems4.11-gcov%{_exeext}
%if "%{gcc_version}" < "4.6.0"
%{_bindir}/i386-rtems4.11-gccbug
%endif

%dir %{_libexecdir}
%dir %{_libexecdir}/gcc
%dir %{_libexecdir}/gcc/i386-rtems4.11
%dir %{_libexecdir}/gcc/i386-rtems4.11/%{gcc_version}
%{_libexecdir}/gcc/i386-rtems4.11/%{gcc_version}/cc1%{_exeext}
%{_libexecdir}/gcc/i386-rtems4.11/%{gcc_version}/collect2%{_exeext}
%if "%{gcc_version}" >= "4.5.0"
%{?with_lto:%{_libexecdir}/gcc/i386-rtems4.11/%{gcc_version}/lto%{_exeext}}
%{_libexecdir}/gcc/i386-rtems4.11/%{gcc_version}/lto-wrapper%{_exeext}
%endif

%files -n rtems-4.11-i386-rtems4.11-gcc-libgcc -f build/files.gcc
%defattr(-,root,root)
%dir %{_prefix}
%dir %{_gcclibdir}
%dir %{_gcclibdir}/gcc
%dir %{_gcclibdir}/gcc/i386-rtems4.11
%dir %{_gcclibdir}/gcc/i386-rtems4.11/%{gcc_version}
%dir %{_gcclibdir}/gcc/i386-rtems4.11/%{gcc_version}/include

%if "%{gcc_version}" > "4.0.3"
%dir %{_gcclibdir}/gcc/i386-rtems4.11/%{gcc_version}/include/ssp
%endif

%if "%{gcc_version}" >= "4.3.0"
%{_gcclibdir}/gcc/i386-rtems4.11/%{gcc_version}/include-fixed
%endif

# ==============================================================
# rtems-4.11-gcc-common
# ==============================================================
%package -n rtems-4.11-gcc-common
Summary:	Base package for rtems gcc and newlib C Library
Group:          Development/Tools
Version:        %{gcc_rpmvers}
%{?_with_noarch_subpackages:BuildArch: noarch}
License:	GPL

Requires(post): 	/sbin/install-info
Requires(preun):	/sbin/install-info

%description -n rtems-4.11-gcc-common
GCC files that are shared by all targets.

%files -n rtems-4.11-gcc-common
%defattr(-,root,root)
%dir %{_prefix}
%dir %{_prefix}/share

%dir %{_infodir}
%ghost %{_infodir}/dir
%{_infodir}/cpp.info*
%{_infodir}/cppinternals.info*
%{_infodir}/gcc.info*
%{_infodir}/gccint.info*
%{_infodir}/gccinstall.info*
%if "%{gcc_version}" >= "4.6.0"
%{_infodir}/libquadmath.info*
%endif

%dir %{_mandir}
%dir %{_mandir}/man7
%{_mandir}/man7/fsf-funding.7*
%{_mandir}/man7/gfdl.7*
%{_mandir}/man7/gpl.7*

%post -n rtems-4.11-gcc-common
  /sbin/install-info --info-dir=%{_infodir} %{_infodir}/cpp.info.gz || :
  /sbin/install-info --info-dir=%{_infodir} %{_infodir}/cppinternals.info.gz || :
  /sbin/install-info --info-dir=%{_infodir} %{_infodir}/gcc.info.gz || :
  /sbin/install-info --info-dir=%{_infodir} %{_infodir}/gccint.info.gz || :
  /sbin/install-info --info-dir=%{_infodir} %{_infodir}/gccinstall.info.gz || :

%preun -n rtems-4.11-gcc-common
if [ $1 -eq 0 ]; then
  /sbin/install-info --delete --info-dir=%{_infodir} %{_infodir}/cpp.info.gz || :
  /sbin/install-info --delete --info-dir=%{_infodir} %{_infodir}/cppinternals.info.gz || :
  /sbin/install-info --delete --info-dir=%{_infodir} %{_infodir}/gcc.info.gz || :
  /sbin/install-info --delete --info-dir=%{_infodir} %{_infodir}/gccint.info.gz || :
  /sbin/install-info --delete --info-dir=%{_infodir} %{_infodir}/gccinstall.info.gz || :
fi

# ==============================================================
# rtems-4.11-i386-rtems4.11-gcc-c++
# ==============================================================
%package -n rtems-4.11-i386-rtems4.11-gcc-c++
Summary:	GCC c++ compiler for i386-rtems4.11
Group:		Development/Tools
Version:        %{gcc_rpmvers}
License:	GPL
Requires:       rtems-4.11-i386-rtems4.11-gcc-libstdc++ = %{gcc_rpmvers}-%{release}

%if "%{_build}" != "%{_host}"
BuildRequires:  rtems-4.11-i386-rtems4.11-gcc-c++ = %{gcc_rpmvers}
%endif

Requires:       rtems-4.11-gcc-common
Requires:       rtems-4.11-i386-rtems4.11-gcc = %{gcc_rpmvers}-%{release}

%description -n rtems-4.11-i386-rtems4.11-gcc-c++
GCC c++ compiler for i386-rtems4.11.


%package -n rtems-4.11-i386-rtems4.11-gcc-libstdc++
Summary:	libstdc++ for i386-rtems4.11
Group:		Development/Tools
Version:        %{gcc_rpmvers}
%{?_with_noarch_subpackages:BuildArch: noarch}
License:	GPL

%description -n rtems-4.11-i386-rtems4.11-gcc-libstdc++
%{summary}


%files -n rtems-4.11-i386-rtems4.11-gcc-c++
%defattr(-,root,root)
%dir %{_prefix}

%dir %{_mandir}
%dir %{_mandir}/man1
%{_mandir}/man1/i386-rtems4.11-g++.1*

%dir %{_bindir}
%{_bindir}/i386-rtems4.11-c++%{_exeext}
%{_bindir}/i386-rtems4.11-g++%{_exeext}

%dir %{_libexecdir}
%dir %{_libexecdir}/gcc
%dir %{_libexecdir}/gcc/i386-rtems4.11
%dir %{_libexecdir}/gcc/i386-rtems4.11/%{gcc_version}
%{_libexecdir}/gcc/i386-rtems4.11/%{gcc_version}/cc1plus%{_exeext}


%files -n rtems-4.11-i386-rtems4.11-gcc-libstdc++ -f build/files.g++
%defattr(-,root,root)
%dir %{_prefix}
%dir %{_gcclibdir}
%dir %{_gcclibdir}/gcc
%dir %{_gcclibdir}/gcc/i386-rtems4.11
%dir %{_gcclibdir}/gcc/i386-rtems4.11/%{gcc_version}
%dir %{_gcclibdir}/gcc/i386-rtems4.11/%{gcc_version}/include
%{_gcclibdir}/gcc/i386-rtems4.11/%{gcc_version}/include/c++



# ==============================================================
# rtems-4.11-i386-rtems4.11-newlib
# ==============================================================
%package -n rtems-4.11-i386-rtems4.11-newlib
Summary:      	C Library (newlib) for i386-rtems4.11
Group: 		Development/Tools
License:	Distributable
Version:	%{newlib_version}
Release:        23%{?dist}
%{?_with_noarch_subpackages:BuildArch: noarch}

Requires:	rtems-4.11-newlib-common

%description -n rtems-4.11-i386-rtems4.11-newlib
Newlib C Library for i386-rtems4.11.

%files -n rtems-4.11-i386-rtems4.11-newlib -f build/files.newlib
%defattr(-,root,root)
%dir %{_exec_prefix}
%dir %{_exec_prefix}/i386-rtems4.11
%{_exec_prefix}/i386-rtems4.11/include

# ==============================================================
# rtems-4.11-newlib-common
# ==============================================================
%package -n rtems-4.11-newlib-common
Summary:	Base package for RTEMS newlib C Library
Group:          Development/Tools
Version:        %{newlib_version}
Release:        23%{?dist}
%{?_with_noarch_subpackages:BuildArch: noarch}
License:	Distributable

Requires(post): 	/sbin/install-info
Requires(preun):	/sbin/install-info

%description -n rtems-4.11-newlib-common
newlib files that are shared by all targets.

%files -n rtems-4.11-newlib-common
%defattr(-,root,root)
%dir %{_prefix}
%dir %{_prefix}/share

%dir %{_infodir}
%ghost %{_infodir}/dir
%{_infodir}/libc.info*
%{_infodir}/libm.info*

%post -n rtems-4.11-newlib-common
  /sbin/install-info --info-dir=%{_infodir} %{_infodir}/libc.info.gz || :
  /sbin/install-info --info-dir=%{_infodir} %{_infodir}/libm.info.gz || :

%preun -n rtems-4.11-newlib-common
if [ $1 -eq 0 ]; then
  /sbin/install-info --delete --info-dir=%{_infodir} %{_infodir}/libc.info.gz || :
  /sbin/install-info --delete --info-dir=%{_infodir} %{_infodir}/libm.info.gz || :
fi

