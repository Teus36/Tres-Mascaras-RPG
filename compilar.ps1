param([string]$Compilador = 'gcc')
$ErrorActionPreference = 'Stop'
Push-Location -LiteralPath $PSScriptRoot
try {
    & $Compilador -std=c99 -Wall -Wextra -Wpedantic -finput-charset=UTF-8 -fexec-charset=UTF-8 -O2 -Isrc src/main.c src/interface.c src/personagem.c src/combate.c src/tabuleiro.c -o tres_mascaras.exe
    if ($LASTEXITCODE -ne 0) { throw 'A compilacao falhou.' }
    Write-Output 'Compilado. Execute: .\tres_mascaras.exe'
} finally {
    Pop-Location
}
