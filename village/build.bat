set BEG=%time%
..\sun\bin\bjam.exe   -j4   -q %*

set END=%time%

echo "BEG: %BEG%  END: %END%"

