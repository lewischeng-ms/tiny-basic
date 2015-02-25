10 REM Lunar Lander
20 REM Modified By Lewis Cheng, Original Author Diomidis Spinellis
30 PRINTLN "You are on the Lunar Lander about to leave the spacecraft."
60 GOSUB 4000
70 GOSUB 1000
80 GOSUB 2000
90 GOSUB 3000
100 H = H - V
110 V = ((V + G) * 10 - U * 2) / 10
120 F = F - U
130 IF H > 0 THEN 80
135 H = 0
140 GOSUB 2000
150 IF V > 5 THEN 200
160 PRINTLN "Congratulations! This was a very good landing."
170 GOSUB 5000
180 GOSUB 10
200 PRINTLN "You have crashed."
210 GOTO 170
1000 REM Initialize
1010 V = 70
1020 F = 500
1030 H = 1000
1040 G = 2
1050 RETURN
2000 REM Print values
2010 PRINTLN "Meter readings"
2015 PRINTLN "------------------"
2020 PRINTLN "Fuel (gal):"
2030 PRINTLN F
2032 IF H <> 0 THEN 2035
2033 JMP = 2100
2034 GOTO 2040
2035 JMP = 2200
2040 GOSUB  JMP
2050 PRINTLN V
2060 PRINTLN "Height (m):"
2070 PRINTLN H
2080 RETURN
2100 PRINTLN "Landing velocity (m/sec):"
2110 RETURN
2200 PRINTLN "Velocity (m/sec):"
2210 RETURN
3000 REM User input
3005 IF F = 0 THEN 3070
3010 PRINTLN "How much fuel will you use?"
3020 INPUT U
3025 IF U < 0 THEN 3090
3030 IF U <= F THEN 3060
3040 PRINTLN "Sorry, you have not got that much fuel!"
3050 GOTO 3010
3060 RETURN
3070 U = 0
3080 RETURN
3090 PRINTLN "No cheating please! Fuel must be >= 0."
3100 GOTO 3010
4000 REM Detachment
4005 PRINTLN "Ready for detachment"
4007 PRINTLN "--- COUNTDOWN ---"
4010 FOR I = 1 TO 11
4020 PRINTLN 11 - I
4025 GOSUB 4500
4030 NEXT I
4035 PRINTLN "You have left the spacecraft."
4037 PRINTLN "Try to land with velocity less than 5 m/sec. "
4040 RETURN
4500 REM Delay
4510 FOR J = 1 TO 5000
4520 NEXT J
4530 RETURN
5000 PRINTLN "Do you want to play again? ( 0 = no, 1 = yes )"
5010 INPUT Y
5020 IF Y = 0 THEN 5040
5030 RETURN
5040 PRINTLN "Have a nice day."
5050 END