read X;
read Y;
while (Y != 0) (
Z = X % Y;
X = Y;
Y = Z;
)
write X;
