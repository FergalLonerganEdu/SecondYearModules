green <- c(1:6, 1:6, 1:6, 1:6, 1:6, 1:6)
green<-rep(1:6, times=6)
red <- sort(green, decreasing = FALSE)
red <- rep(1:6, times=rep(6,6))
dice <- data.frame(red, green)
dice$sum <- dice$red + dice$green
counts <- table(dice$sum)
counts <- data.frame(counts)
counts$prob <- counts$Freq / 36

sample50 <- sample(dice$sum, 50, replace = TRUE)
prob50_table <- table(sample50)
prob50 <- data.frame(prob50_table)
prob50$relfreq <- prob50$Freq/50
prob50

sample500 <- sample(dice$sum, 500, replace = TRUE)
prob500_table <- table(sample500)
prob500 <- data.frame(prob500_table)
prob500$relfreq <- prob500$Freq/500
prob500

bin_samp <- rbinom(n=1, size=100, prob=0.55)
dbinom(x=58, size=100, prob=0.55 )
pbinom(q=58, size=100, prob=0.55, lower.tail=TRUE)
pbinom(q=58, size=100, prob=0.55, lower.tail=FALSE)

dhyper(x=6, m=6, n=39, k=6)
# Look up the help files to see what x, m, n and k are.
lotto <- rhyper(nn=8320, m=6, n=39, k=6)
table(lotto)
barplot(table(lotto))
barplot(table(lotto),xlab = "matches", ylab = "frequency", main = "barchart of lotto results from 100000000 attempts")

rpois(n=7, lambda=90000)
sum(rpois(n=7, lambda=90000))
revenue <- (rpois(n=7, lambda=90000) * 2.1)
ppois(q=89000, lambda=90000)
dpois(x=89000, lambda=90000)

