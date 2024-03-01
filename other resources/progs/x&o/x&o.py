def reset():
  global win, currentNUM, lastTurn, A1, A2, A3, B1, B2, B3, C1, C2, C3
  win = '-'
  lastTurn = 'O'
  A1 = '-'
  A2 = '-'
  A3 = '-'
  B1 = '-'
  B2 = '-'
  B3 = '-'
  C1 = '-'
  C2 = '-'
  C3 = '-'

# Opiši funkcijo ...
def Xturn():
  global win, currentNUM, lastTurn, A1, A2, A3, B1, B2, B3, C1, C2, C3
  currentNUM = input('X je na vrsti' + ' ' + str(A1 + (A2 + A3)) + '                                                                                   ' + str(B1 + (B2 + B3)) + '                                                                             ' + str(C1 + (C2 + C3))).upper()
  if currentNUM == 'A1' and A1 == '-':
    A1 = 'X'
  elif currentNUM == 'A2' and A2 == '-':
    A2 = 'X'
  elif currentNUM == 'A3' and A3 == '-':
    A3 = 'X'
  elif currentNUM == 'B1' and B1 == '-':
    B1 = 'X'
  elif currentNUM == 'B2' and B2 == '-':
    B2 = 'X'
  elif currentNUM == 'B3' and B3 == '-':
    B3 = 'X'
  elif currentNUM == 'C1' and C1 == '-':
    C1 = 'X'
  elif currentNUM == 'C2' and C2 == '-':
    C2 = 'X'
  elif currentNUM == 'C3' and C3 == '-':
    C3 = 'X'
  else:
    print('vpisite CRKO in STEVILO (A1,A2,...)')
    Xturn()

# Opiši funkcijo ...
def Oturn():
  global win, currentNUM, lastTurn, A1, A2, A3, B1, B2, B3, C1, C2, C3
  currentNUM = input('O je na vrsti' + ' ' + str(A1 + (A2 + A3)) + '                                                                                   ' + str(B1 + (B2 + B3)) + '                                                                             ' + str(C1 + (C2 + C3))).upper()
  if currentNUM == 'A1' and A1 == '-':
    A1 = 'O'
  elif currentNUM == 'A2' and A2 == '-':
    A2 = 'O'
  elif currentNUM == 'A3' and A3 == '-':
    A3 = 'O'
  elif currentNUM == 'B1' and B1 == '-':
    B1 = 'O'
  elif currentNUM == 'B2' and B2 == '-':
    B2 = 'O'
  elif currentNUM == 'B3' and B3 == '-':
    B3 = 'O'
  elif currentNUM == 'C1' and C1 == '-':
    C1 = 'O'
  elif currentNUM == 'C2' and C2 == '-':
    C2 = 'O'
  elif currentNUM == 'C3' and C3 == '-':
    C3 = 'O'
  else:
    print('vpisite CRKO in STEVILO (A1,A2,...)')
    Oturn()

# Opiši funkcijo ...
def check():
  global win, currentNUM, lastTurn, A1, A2, A3, B1, B2, B3, C1, C2, C3
  if A1 == A2 and A2 == A3:
    win = A1
  elif B1 == B2 and B2 == B3:
    win = B1
  elif C1 == C2 and C2 == C3:
    win = C1
  elif A1 == B1 and B1 == C1:
    win = A1
  elif A2 == B2 and B2 == C2:
    win = A2
  elif A3 == B3 and B3 == C3:
    win = A3
  elif A1 == B2 and B2 == C3:
    win = A1
  elif A3 == B2 and B2 == C1:
    win = A3


reset()
while win == '-' and (A1 == '-' or A2 == '-' or A3 == '-' or B1 == '-' or B2 == '-' or B3 == '-' or C1 == '-' or C2 == '-' or C3 == '-'):
  if 'X' == lastTurn:
    Oturn()
    lastTurn = 'O'
  else:
    Xturn()
    lastTurn = 'X'
  check()
print(A1 + (A2 + A3))
print(B1 + (B2 + B3))
print(C1 + (C2 + C3))
print('ZMAGAL JE:')
print(win)