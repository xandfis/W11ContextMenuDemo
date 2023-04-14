## Overview
This repo contains a simplified version of the official ["PhotoStoreDemo" Sparse Package sample code](https://github.com/microsoft/AppModelSamples/tree/master/Samples/SparsePackages). 

This version focuses on how to add a very simple Windows 11 context menu extension.

You can learn more about the Windows 11 context menu on my [post](https://blogs.windows.com/windowsdeveloper/2021/07/19/extending-the-context-menu-and-share-dialog-in-windows-11/) on the Windows Developer blog.

## Instructions

https://www.youtube.com/watch?v=MedIpcS4Hcg


## Reference Commands

The YouTube tutorial shows using these commands, which you can copy and use in your environment. Paths may be different on your own machine. Remember to replace <YOUR PASSWORD> with a new password - this isn't your Windows account password. 
### Make a Cert 
In a PowerShell window:
```
New-SelfSignedCertificate -Type Custom -Subject "CN=Contoso" -KeyUsage DigitalSignature -FriendlyName "SelfSignCert" -CertStoreLocation "Cert:\CurrentUser\My" -TextExtension @("2.5.29.37={text}1.3.6.1.5.5.7.3.3", "2.5.29.19={text}")
```
### Export the Cert to a PFX file
```
$password = ConvertTo-SecureString -String <YOUR PASSWORD> -Force -AsPlainText 
Export-PfxCertificate -cert "Cert:\CurrentUser\My\<Certificate Thumbprint>" -FilePath <FilePath>.pfx -Password $password
```
### Make an MSIX Package
In a CMD window:
```
"C:\Program Files (x86)\Windows Kits\10\bin\10.0.22000.0\x64\makeappx.exe" pack /d C:\src\AppModelSamples-master\Samples\SparsePackages\ContextMenuRegistrationPkg /p "C:\src\mypackage.msix" /nv
```
### Sign the MSIX Package
```
"C:\Program Files (x86)\Windows Kits\10\bin\10.0.22000.0\x64\signtool.exe" sign /fd SHA256 /a /f "C:\src\MyPFX.pfx" /p <YOUR PASSWORD> "C:\src\mypackage.msix"
```
