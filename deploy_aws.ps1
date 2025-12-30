# Secure Password Vault - AWS Windows Deployment Helper
# Run this on your local Windows machine to prepare files for AWS deployment

Write-Host "=========================================" -ForegroundColor Cyan
Write-Host "Secure Password Vault - AWS Deployment Preparation" -ForegroundColor Cyan
Write-Host "=========================================" -ForegroundColor Cyan
Write-Host ""

# Configuration
$ProjectDir = Get-Location
$OutputZip = "password_vault_deploy.zip"

Write-Host "Step 1: Building project locally..." -ForegroundColor Green

# Create build directory
if (!(Test-Path "build")) {
    New-Item -ItemType Directory -Path "build" | Out-Null
}

Set-Location build

# Run CMake
Write-Host "Running CMake..." -ForegroundColor Yellow
cmake .. -G "MinGW Makefiles"

if ($LASTEXITCODE -ne 0) {
    Write-Host "CMake failed. Make sure CMake and MinGW are installed." -ForegroundColor Red
    exit 1
}

# Build
Write-Host "Building..." -ForegroundColor Yellow
cmake --build .

if ($LASTEXITCODE -ne 0) {
    Write-Host "Build failed. Check the error messages above." -ForegroundColor Red
    exit 1
}

Set-Location ..

Write-Host "`nStep 2: Creating deployment package..." -ForegroundColor Green

# Files to include
$FilesToInclude = @(
    "src/*",
    "include/*",
    "web/*",
    "CMakeLists.txt",
    "README.md",
    "DEPLOYMENT.md",
    "deploy_aws.sh"
)

# Create zip file
if (Test-Path $OutputZip) {
    Remove-Item $OutputZip
}

Write-Host "Compressing files..." -ForegroundColor Yellow
Compress-Archive -Path $FilesToInclude -DestinationPath $OutputZip

Write-Host "`n=========================================" -ForegroundColor Cyan
Write-Host "Deployment package created successfully!" -ForegroundColor Green
Write-Host "=========================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "Package: $OutputZip" -ForegroundColor Yellow
Write-Host ""
Write-Host "Next steps:" -ForegroundColor Cyan
Write-Host "  1. Launch an AWS EC2 instance (Ubuntu 22.04 LTS recommended)"
Write-Host "  2. Security Group: Open ports 22 (SSH), 80 (HTTP), 443 (HTTPS)"
Write-Host "  3. Connect to your EC2 instance via SSH"
Write-Host "  4. Upload the deployment package:"
Write-Host "     scp $OutputZip ubuntu@YOUR_EC2_IP:~/" -ForegroundColor Yellow
Write-Host "  5. On the EC2 instance:"
Write-Host "     unzip $OutputZip" -ForegroundColor Yellow
Write-Host "     cd secure_password_vault" -ForegroundColor Yellow
Write-Host "     chmod +x deploy_aws.sh" -ForegroundColor Yellow
Write-Host "     ./deploy_aws.sh" -ForegroundColor Yellow
Write-Host ""
Write-Host "EC2 Instance Recommendations:" -ForegroundColor Cyan
Write-Host "  - Type: t2.micro or t3.micro (free tier eligible)"
Write-Host "  - Storage: 20GB gp3"
Write-Host "  - OS: Ubuntu 22.04 LTS"
Write-Host "  - Region: Choose closest to your users"
Write-Host ""
Write-Host "Don't forget to:" -ForegroundColor Yellow
Write-Host "  - Point your domain's DNS to the EC2 instance IP"
Write-Host "  - Configure an Elastic IP for a static address"
Write-Host "  - Set up automated backups"
Write-Host ""
