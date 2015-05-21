
# =============================================================================
#    This script can take an optional parameter for SLURM configuration.
#    In this case, you should execute the script as follows:
#
#         ./scripts/make_scripts.sh <SLURM name>
# =============================================================================
#!/bin/bash

cd scripts/lua_scripts/
lua makeInit.lua > 0-init.sh

if [ "$#" -ne 1 ]; then
     lua makeOL.lua > 1-ol.sh
     lua makeRE.lua > 2-re.sh

else
     lua makeOL.lua $1 create >  1-ol.sh
     lua makeOL.lua $1 submit >> 1-ol.sh
     
     lua makeRE.lua $1 create >  2-re.sh
     lua makeRE.lua $1 submit >> 2-re.sh

fi

lua makeExport.lua > 3-export.sh

chmod +x 0-init.sh
chmod +x 1-ol.sh
chmod +x 2-re.sh
chmod +x 3-export.sh

mv *sh ../
cd ../
