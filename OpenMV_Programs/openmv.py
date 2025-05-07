import sensor, image, time, machine

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QQVGA)
sensor.skip_frames(time=2000)
sensor.set_auto_gain(False)
sensor.set_auto_whitebal(False)

uart = machine.UART(3, 115200)

YELLOW_THRESHOLD = (20, 85, -30, 15, 20, 80)



IMG_WIDTH = sensor.width()
SECTION_WIDTH = IMG_WIDTH // 3

while True:
    try:
        img = sensor.snapshot()
        blobs = img.find_blobs([YELLOW_THRESHOLD], pixels_threshold=100, area_threshold=100)

        position = "Ninguno"

        if blobs:
            biggest_blob = max(blobs, key=lambda b: b.area())
            img.draw_rectangle(biggest_blob.rect(), color=(255, 0, 0))
            img.draw_cross(biggest_blob.cx(), biggest_blob.cy(), color=(0, 255, 0))

            if biggest_blob.cx() < SECTION_WIDTH:
                position = "Izquierda"
            elif biggest_blob.cx() < 2 * SECTION_WIDTH:
                position = "Centro"
            else:
                position = "Derecha"


        print(position)
        uart.write(position + "\n")

        time.sleep_ms(10)  # ¡Más rápidooooooooooooooooo!

    except Exception as e:
        print("Error:", str(e))
        sensor.reset()
        sensor.set_pixformat(sensor.RGB565)
        sensor.set_framesize(sensor.QQVGA)
        sensor.skip_frames(time=2000)
