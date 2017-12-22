#!/bin/bash
bold=$(tput bold)
normal=$(tput sgr0)

cd shaders

for D in *; do
    if [ -d "${D}" ]; then
        echo "Process ${bold}${D}${normal} :"
		rm ${D}.zip
		zip -r ${D}.zip ${D}
        echo ""
    fi
done

cd ..

