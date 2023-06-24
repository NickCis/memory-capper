_buildfolder=memory-capper-build
pkgname=memory-capper
pkgver=v1.0.0
pkgrel=1
pkgdesc='Kill process that consumes most memory when a threshold is reached'
arch=('i686' 'x86_64')
license=('MIT')
url='http://github.com/nickcis/memory-capper/'
depends=()
makedepends=('automake' 'libtool' 'gcc' 'sed')
source=("$_buildfolder::git+https://github.com/nickcis/memory-capper.git")
sha256sums=('SKIP')

pkgver() {
    cd "$srcdir/$_buildfolder"
    git log --format="%ad.%h" --date=format:'%Y%m%d%H%M%S' -n 1
}

build() {
    cd "$srcdir/$_buildfolder"
    ./autogen.sh
    ./configure --prefix=/usr \
        --sysconfdir=/etc \
     make
}

package() {
    cd "$srcdir/$_buildfolder"
    make DESTDIR="${pkgdir}" install
}

