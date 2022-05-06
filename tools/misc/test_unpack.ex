defmodule TestUnpack do

  def test_packet(), do: File.read("head.pds")

  def data_zone(<<_prim::bitstring-size(48), _sec::bitstring-size(72), data::bitstring>>), do: data

  def data_field_plus_check(<<_::bitstring-size(24), data_field::bitstring>>), do: data_field

  def get_checksum(data_field_plus_check) do
    data_field_len = bit_size(data_field_plus_check) - 12
    <<data_field::bitstring-size(data_field_len), checksum1::4, checksum2::8>> = data_field_plus_check
    <<checksum::12>> = <<checksum1::4, checksum2::8>>
    checksum
  end

  def calc_checksum(<<>>), do: 0

  def calc_checksum(<<x1::8, x2::4, y1::4, y2::8, xs::bitstring>>) do
    <<x::12>> = <<x1::8, x2::4>>
    <<y::12>> = <<y1::4, y2::8>>
    x + y + calc_checksum(xs)
  end

  def test() do
    {:ok, packet} = test_packet()
    data_field_plus_check = packet |> data_zone() |> data_field_plus_check()
    check1 = get_checksum(data_field_plus_check)
    check2 = calc_checksum(data_field_plus_check)
    check2 = check2 - check1
    check2 = Bitwise.bsr(check2, 4)
    check2 = Bitwise.band(check2, 0xFFF)
    IO.inspect(check1 |> Integer.to_string(16))
    IO.inspect(check2 |> Integer.to_string(16))
    nil
  end

end
