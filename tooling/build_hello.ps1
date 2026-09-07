#requires -Version 7.6

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
$PSNativeCommandUseErrorActionPreference = $true

$projectRoot = Split-Path $PSScriptRoot -Parent
$source = Join-Path $projectRoot 'src' 'asm' 'hello.asm'
$outputDir = Join-Path $projectRoot 'build' 'hello'
$object = Join-Path $outputDir 'hello.obj'
$executable = Join-Path $outputDir 'hello.exe'

mkdir $outputDir -Force | Out-Null # Out-Null hides the output

Write-Host 'Assembling hello.asm with FASM'
& fasm $source $object

Write-Host 'Linking hello.obj with the C runtime'
& clang $object -o $executable

Write-Host 'Running hello.exe'
& $executable
