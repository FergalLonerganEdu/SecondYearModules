firedamage <- read.table("firedamage.txt", header = TRUE)
firedamage
plot(firedamage$distance, firedamage$damage, main="Fire Damage vs.Distance", xlab="Distance (km)", ylab="Damage (1000 euro)")

cor(firedamage$distance, firedamage$damage)

reg <- lm(damage ∼ distance, data=firedamage)
summary(reg)

distance <- 4.2
damage <- (4.9193*distance)+(10.2779)

predict(reg, data.frame(distance=4.2))

plot(firedamage$distance, firedamage$damage, main = "Fire Damage vs Distance", xlab = "Distance (km)", ylab = "Damage (1000 euro)") 
lines(firedamage$distance, reg$fitted.values)



ojuice <- read.table("ojuice.txt")
ojuice <- data.frame(ojuice)

ojuice

sweetness <- (ojuice$V2)
pectin <- (ojuice$V3)

ojuice <- data.frame(sweetness, pectin)
ojuice

cor(ojuice$sweetness, ojuice$pectin)

reg <- lm(pectin ∼ sweetness, data=ojuice)
summary(reg)

plot(ojuice$sweetness, ojuice$pectin, main = "pectin vs sweetness", xlab = "sweetness", ylab = "pectin") 
lines(ojuice$sweetness, reg$fitted.values)
predict(reg, data.frame(sweetness=243))
