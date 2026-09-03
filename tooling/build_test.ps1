$testDir = "./test"
$binGen = "../build/bingen.exe"

mkdir $testDir -Force | Out-Null
Push-Location $testDir
try {
	Write-Host "Running bingen.exe to generate square.o"
	& $binGen &&

	Write-Host "Compiling object file test.o"
	clang++ -c -std=c++23 ../src/test.cpp -o test.o &&

	Write-Host "Linking test & square"
	clang++ test.o square.o -o test.exe &&

	Write-Host "Running test.exe"
	./test.exe
	exit $LASTEXITCODE
}
finally {
	Pop-Location
}
