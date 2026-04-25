# The Shell Power - Questions & Answers

---

## Section 0: Intro

**Part 0.** Intro challenge to unlock the series.
> Answer: `CyberSphere{The_Shell_Power_Begins}`

---

## Section 1: Host Identification

**Q1.** What is the computer name of the system?
> Answer: `CyberSphere{DESKTOP-6F1LGKO}`

**Q2.** What is the OS build number of the installed Windows system?
> Answer: `CyberSphere{19045}`

**Q3.** What is the victim's username?
> Answer: `CyberSphere{someone}`

**Q4.** What version of WinRAR is installed? (eg. CyberSphere{7.20})
> Answer: `CyberSphere{7.20}`

---

## Section 2: Download and Execution Timeline

**Q5.** How long did the Discord installer download take? (seconds)
> Answer: `CyberSphere{415}`

**Q6.** When did the Discord installer start? (Format: YYYY-MM-DD HH:MM:SS UTC)
> Answer: `CyberSphere{2026-04-15 11:57:26}`

---

## Section 3: Malicious URL Activity

**Q7.** What is the last URL visited before infection?
> Answer: `CyberSphere{https://recaptcha-verification-step.netlify.app/}`

**Q8.** What is the timestamp of that visit? (Format: YYYY-MM-DD HH:MM:SS UTC)
> Answer: `CyberSphere{2026-04-15 12:46:36}`

**Q9.** What is the URL of the script executed via Run dialog?
> Answer: `CyberSphere{https://recaptcha-verification-step.netlify.app/Verify.ps1}`

---

## Section 4: Payload and C2

**Q10.** What is the SHA-256 of the reverse shell payload?
> Answer: `CyberSphere{2dc08a81a579ddb1035e16378fc21e57f5df0590b1c88a2213e7d91fa7bb01d7}`

**Q11.** What IP and port did the reverse shell connect to? (eg. 1.1.1.1:9999)
> Answer: `CyberSphere{52.16.191.79:7039}`

---

## Section 5: Persistence

**Q12.** What is the name of the persistence mechanism (fake service)?
> Answer: `CyberSphere{SystemHealthMonitoring}`

**Q13.** What is the final payload URL used for persistence?
> Answer: `CyberSphere{http://52.16.191.79:6969/HealthCheck.ps1}`

---

## Section 6: Trolling Note

**Q14.** The attacker trolled the victim by leaving a note. What is the content of this note?
> Answer: `CyberSphere{RIGGED KEKW}`
