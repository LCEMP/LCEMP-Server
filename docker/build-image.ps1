param(
    [string]$Image = "lcemp-server:local",
    [int]$BuildJobs = 2,
    [string]$GitHubRepo = $env:GITHUB_REPOSITORY,
    [string]$GitHubReleaseTag = $env:GITHUB_RELEASE_TAG,
    [string]$GitHubAssetName = $(if ($env:GITHUB_ASSET_NAME) { $env:GITHUB_ASSET_NAME } else { "MinecraftDedicatedServer" }),
    [switch]$ForceFullBuild
)

$ErrorActionPreference = "Stop"

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$RepoRoot = Resolve-Path (Join-Path $ScriptDir "../..")
$PrebuiltBinary = Join-Path $RepoRoot "build/MinecraftDedicatedServer"

function Invoke-DockerPrebuiltBuild {
    Write-Host "Using prebuilt Linux server binary: $PrebuiltBinary"
    Push-Location $RepoRoot
    try {
        docker build `
            -f Minecraft.Server/docker/Dockerfile.prebuilt `
            -t $Image `
            .
        if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
    } finally {
        Pop-Location
    }
}

function Invoke-FullDockerBuild {
    Push-Location $RepoRoot
    try {
        docker build `
            -f Minecraft.Server/docker/Dockerfile `
            --build-arg BUILD_JOBS=$BuildJobs `
            -t $Image `
            .
        if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
    } finally {
        Pop-Location
    }
}

function Try-DownloadGitHubBinary {
    if ([string]::IsNullOrWhiteSpace($GitHubRepo)) {
        return $false
    }

    $releasePath = if ([string]::IsNullOrWhiteSpace($GitHubReleaseTag) -or $GitHubReleaseTag -eq "latest") {
        "latest"
    } else {
        "tags/$GitHubReleaseTag"
    }

    $releaseUrl = "https://api.github.com/repos/$GitHubRepo/releases/$releasePath"
    Write-Host "Looking for GitHub release asset '$GitHubAssetName' at $releaseUrl"

    try {
        $headers = @{
            Accept = "application/vnd.github+json"
            "X-GitHub-Api-Version" = "2022-11-28"
            "User-Agent" = "lcemp-docker-build"
        }
        $release = Invoke-RestMethod -Uri $releaseUrl -Headers $headers
        $asset = $release.assets | Where-Object { $_.name -eq $GitHubAssetName } | Select-Object -First 1
        if (-not $asset) {
            Write-Host "GitHub release found, but asset '$GitHubAssetName' was not present."
            return $false
        }

        $buildDir = Split-Path -Parent $PrebuiltBinary
        New-Item -ItemType Directory -Force -Path $buildDir | Out-Null
        Invoke-WebRequest -Uri $asset.browser_download_url -OutFile $PrebuiltBinary -Headers @{ "User-Agent" = "lcemp-docker-build" }
        Write-Host "Downloaded Linux server binary from GitHub: $($asset.browser_download_url)"
        return $true
    } catch {
        Write-Host "GitHub binary download failed: $($_.Exception.Message)"
        return $false
    }
}

if (-not $ForceFullBuild -and (Test-Path $PrebuiltBinary)) {
    Invoke-DockerPrebuiltBuild
} else {
    if ($ForceFullBuild) {
        Write-Host "ForceFullBuild set; running full Docker build."
        Invoke-FullDockerBuild
    } else {
        Write-Host "No prebuilt Linux server binary found at: $PrebuiltBinary"
        if (Try-DownloadGitHubBinary -and (Test-Path $PrebuiltBinary)) {
            Invoke-DockerPrebuiltBuild
        } else {
            Write-Host "Running full Docker build."
            Invoke-FullDockerBuild
        }
    }
}
