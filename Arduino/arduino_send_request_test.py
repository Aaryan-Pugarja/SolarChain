import requests

url = "http://192.168.0.19:8080/transfer"

data = {
    "units": 10
}

response = requests.post(url, json=data)

print("Status code:", response.status_code)
print("Response:", response.json())
