stack = []
def y(n):
	if n>=10:
		y(n//10)
	else:
		stack.append(n)
y(80)
print(stack)