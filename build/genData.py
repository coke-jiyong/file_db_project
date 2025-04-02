import random

#데이터 생성 스크립트
with open("user_data.txt", "w") as file:
    for i in range(1, 1001):
        name = f"user_{i}"
        age = random.randint(20, 35)
        gender = random.choice(["M", "F"])
        file.write(f"./db_cli user add {name} {age} {gender}\n")

