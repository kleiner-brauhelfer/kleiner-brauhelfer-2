# Deployment of kleiner-brauhelfer-2 on Gentoo Linux
1. Configure a local overlay
Run:

    eselect repository create local

2. Copy ebuild to local overlay

    mkdir -p /var/db/repos/local/sci-chemistry/kleiner-brauhelfer/
    cp deployment/gentoo-linux/*.ebuild /var/db/repos/local/sci-chemistry/kleiner-brauhelfer/

3. Create manifests

    ebuild /var/db/repos/local/sci-chemistry/kleiner-brauhelfer/*.ebuild digest

3. Install

    emerge -av sci-chemistry/kleiner-brauhelfer
