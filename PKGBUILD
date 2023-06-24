pkgname=memory-capper
pkgver=v1.0.0
pkgrel=1
pkgdesc='Kill process that consumes most memory when a threshold is reached'
arch=('i686' 'x86_64')
license=('MIT')
url='http://github.com/nickcis/memory-capper/'
depends=()
makedepends=('automake' 'libtool' 'gcc' 'sed')
source=("memory-capper::git+https://github.com/nickcis/memory-capper.git")
sha256sums=('SKIP')

pkgver() {
    cd "$srcdir/memory-capper"
    git log --format="%H" -n 1
}

build() {
    cd "$srcdir/memory-capper"
    ./autogen.sh
    ./configure --prefix=/usr \
        --sysconfdir=/etc \
     make
}

package() {
    cd "$srcdir/memory-capper"
    make DESTDIR="${pkgdir}" install
}

