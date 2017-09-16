set BEG=%time%
 ..\sun\bin\bjam.exe  -j8     -q %*

set END=%time%

echo "BEG: %BEG%  END: %END%"

