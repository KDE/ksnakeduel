# SPDX-FileCopyrightText: 2022 Friedrich W. H. Kossebau <kossebau@kde.org>
#
# SPDX-License-Identifier: BSD-3-Clause

function(install_theme id)
    set(svgz "${CMAKE_CURRENT_BINARY_DIR}/${id}.svgz")
    generate_svgz(${id}.svg ${svgz} "theme-")
    install(
        FILES
            ${id}.desktop
            ${id}.png
            ${svgz}
        DESTINATION ${KDE_INSTALL_DATADIR}/ksnakeduel/themes
    )
endfunction()

install_theme(default)
install_theme(neon)
