EAPI=7
inherit desktop qmake-utils
DESCRIPTION="Der kleine Brauhelfer ist ein Hilfsprogramm für Hobbybrauer zum Erstellen und Verwalten von Biersuden"
HOMEPAGE="http://kleiner-brauhelfer.github.io"

if [[ ${PV} != 9999 ]]; then
	SRC_URI="https://github.com/kleiner-brauhelfer/kleiner-brauhelfer-2/archive/refs/tags/v${PV}.tar.gz -> ${P}.tar.gz"
else
	inherit git-r3
	EGIT_REPO_URI="https://github.com/kleiner-brauhelfer/kleiner-brauhelfer-2.git"
fi

LICENSE="GPL-3"
SLOT="0"
KEYWORDS="amd64 x86"
IUSE=""

DEPEND="
	dev-qt/qtwebengine:6	
	dev-qt/qtsvg:6
"

RDEPEND="${DEPEND}"
BDEPEND="${DEPEND}"


S="${WORKDIR}/${PN}-2-${PV}"

src_unpack() {
	unpack "${A}"
	cd "${S}"
}

src_configure() {
	eqmake6 kleiner-brauhelfer-2.pro PREFIX="${EPREFIX}"/usr INSTALL_ROOT=.
}

src_install() {
	doicon "${S}/deployment/kleiner-brauhelfer-2.svg"
	domenu "${S}/deployment/linux/64bit/deb/kleiner-brauhelfer-2.desktop"
	dobin "${S}/bin/kleiner-brauhelfer-2"
}

