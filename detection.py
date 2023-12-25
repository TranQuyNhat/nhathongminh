# detection.py
import dweepy
from threading import Event, Thread
import time
import pyodbc
import joblib
import os
import sys


def load_knn_model():
    # Kiểm tra xem tệp mô hình có tồn tại không
    model_file = 'knn_model.joblib'
    if os.path.exists(model_file):
        return joblib.load(model_file)
    else:
        print("Không tìm thấy tệp mô hình.")
        return None

def get_latest_dweet_thread(thing_name, event, knn_model):
    # Kết nối đến cơ sở dữ liệu SQL Server
    conn = pyodbc.connect('Driver={SQL Server};'
                          'Server=DESKTOP-HJ3ORQ9\QUYNHAT;'
                          'Database=SmartHome;'
                          'Trusted_Connection=yes;'
                          )
    cursor = conn.cursor()

    last_time = ""  # Thêm biến để theo dõi thời gian của dữ liệu trước đó

    while not event.is_set():
        try:
            # Lấy dữ liệu mới nhất từ dweet.io
            url = dweepy.get_latest_dweet_for(thing_name)
            dweet_data = url[0]['content']
            longdate = url[0]['created']

            # Extract thông tin thời gian từ dữ liệu nhận được
            date = longdate[:10]
            time_stamp = longdate[11:19]

            # Extract các giá trị từ dữ liệu nhận được từ dweet.io
            device_name = thing_name  # Sử dụng tên của thing_name làm device_name
            NhietDo = float(dweet_data.get("NhietDo", 0))
            DoAm = int(dweet_data.get("DoAm", 0))
            Gas = int(dweet_data.get("Gas", 0))
            AmThanh = int(dweet_data.get("AmThanh", 0))
            AnhSang = int(dweet_data.get("AnhSang", 0))

            # In ra các thông số cảm biến với định dạng chuỗi
            print(f"Nhiet do: {NhietDo:.1f}, Do am: {DoAm}, Gas: {Gas}, Am thanh: {AmThanh}, Anh sang: {AnhSang}")

            # Dự đoán nhãn từ mô hình KNN
            prediction = knn_model.predict([[NhietDo, DoAm, Gas, AmThanh, AnhSang]])
            print(f"Truong hop nhan dien duoc: {prediction[0]}")
            print()

            # Kiểm tra xem liệu dữ liệu đã được lưu chưa để tránh trùng lặp
            if last_time != longdate:
                # Thực hiện lệnh SQL để chèn dữ liệu vào cơ sở dữ liệu
                cursor.execute(
                    '''
                    INSERT INTO SecurityLog(Device_Name, Time, NhietDo, DoAm, Gas, AmThanh, AnhSang)
                    VALUES(?, ?, ?, ?, ?, ?, ?)
                    ''',
                    (device_name, f"{date} {time_stamp}", NhietDo, DoAm, Gas, AmThanh, AnhSang)
                )
                conn.commit()
                last_time = longdate

        except Exception as e:
            print(f"Error: {e}")
            print("Missing a row!")
        time.sleep(1)

# Main function
def main():
    # Tải mô hình từ tệp
    knn_model = load_knn_model()
    if knn_model is not None:
        # Tạo đối tượng Event
        event = Event()
        
        # Tạo và lấy dữ liệu từ dweet.io
        get_data_thread = Thread(target=get_latest_dweet_thread,
                                args=("dnu_cntt1502_nhom10", event, knn_model))
        get_data_thread.start()

        count = 0
        while count < 3:
            count += 1
            print("Count=", count)
            time.sleep(1)

        stop = input("Ket thuc (y/n)? ")
        if stop.lower() == "y":
            print("Main thread finished.")
            # Đặt sự kiện để dừng luồng lấy dữ liệu
            event.set()

if __name__ == '__main__':
    main()