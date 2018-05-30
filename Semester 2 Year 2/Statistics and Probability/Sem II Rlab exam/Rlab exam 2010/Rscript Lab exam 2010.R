# Q2 a
pnorm(4,mean =3, sd = 2)

#Q2 b
a<-pnorm(3.5,mean =3, sd = 2)
b<-pnorm(2.5,mean =3, sd = 2)
a-b

#Q2 c
c<-pnorm(2, mean =3, sd=2)
d<- 1-c
1-pbinom(q=1,size=3,prob=d,lower.tail=T)

#Q3 a 
coinA <- c(2:3,2:3)
coinB <- sort(coinA, decreasing = FALSE)
two <- data.frame(coinA, coinB)
two$sum <- two$coinA + two$coinB
counts <- table(two$sum)
counts <- data.frame(counts)
counts$prob <- counts$Freq / 4
counts$prob
counts

#Q3 b
summary(two$sum)
sd(two$sum)
mean(two$sum)

#Q4 a
time <- c(15.1,16.2,15.7,16.1,16.6,14.9,15.5,15.6,14.3,13.9,13.8,13.6,14.5,13.8,14.1,14.9)
gender <- c(1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2)
table <- data.frame(gender,time)
table

#Q4 b
summary(time)
summary(table$time[table$gender == 1])
summary(table$time[table$gender == 2])

#Q4c
hist(table$time[table$gender == 1], breaks=5)
hist(table$time[table$gender == 2], breaks=5)
