using System.Collections;
using System.Collections.Generic;
using FlanEngine;

public class TestScript : FlanScript
{
    bool firstTime = true;
    List<GameObject> childs = new List<GameObject>();

    public float rotSpeed = 150f;
    public float movSpeed = 50f;

    public GameObject instance;
    public GameObject instance2;

    public double patata_double = 1;
    public sbyte p_sbyte = 1;
    public byte p_byte = 1;
    public short p_int16 = 1;
    public ushort p_uint16 = 1;
    public int p_int = 1;
    public uint p_uint = 1;
    public long p_long = 1;
    public ulong p_ulong = 1;
    public char p_char = 'c';


    public Transform transformTest;

    //Use this method for initialization
    public override void Awake()
    {
        Debug.Log("Instantiating GameObject");
        GameObject instance = GameObject.Instantiate();
        instance.name = "Awake " + childs.Count.ToString();
    }

    //Called every frame
    public override void Update()
    {
        Debug.Log(patata_double.ToString());
        Debug.Log(p_byte.ToString());
        Debug.Log(p_int.ToString());
        Debug.Log(p_int16.ToString());
        Debug.Log(p_long.ToString());
        Debug.Log(p_sbyte.ToString());
        Debug.Log(p_uint.ToString());
        Debug.Log(p_uint16.ToString());
        Debug.Log(p_ulong.ToString());

        if (transformTest != null)
        {
            Debug.ClearConsole();
            Debug.Log("My transformTest's name is " + transformTest.gameObject.name);
            Debug.Log("My transformTest's position is " + transformTest.position.ToString());
        }

        if(Input.GetKey(KeyCode.KEY_E))
        {
            transform.rotation.Rotate(Vector3.up, -rotSpeed * Time.deltaTime);
        }

        if (Input.GetKey(KeyCode.KEY_Q))
        {
            transform.rotation.Rotate(Vector3.up, +rotSpeed * Time.deltaTime);
        }

        if(Input.GetKeyDown(KeyCode.KEY_1))
        {
            Debug.Log("Instantiating GameObject");
            GameObject instance = GameObject.Instantiate();
            childs.Add(instance);
            instance.name = "Child: " + childs.Count.ToString();
        }

        if (Input.GetKeyDown(KeyCode.KEY_2))
        {
            if(childs.Count != 0)
            {
                Debug.Log("Deleting GameObject");
                GameObject toDelete = childs[childs.Count - 1];
                Destroy(toDelete);
                childs.Remove(toDelete);
            }
        }

        if (Input.GetKey(KeyCode.KEY_W))
        {
            transform.position += transform.forward * movSpeed * Time.deltaTime;
        }
        if (Input.GetKey(KeyCode.KEY_A))
        {
            transform.position += transform.right * movSpeed * Time.deltaTime;
        }
        if (Input.GetKey(KeyCode.KEY_S))
        {
            transform.position -= transform.forward * movSpeed * Time.deltaTime;
        }
        if (Input.GetKey(KeyCode.KEY_D))
        {
            transform.position -= transform.right * movSpeed * Time.deltaTime;
        }
    }

    public override void OnEnable()
    {
        Debug.Log("Instantiating GameObject");
        GameObject instance = GameObject.Instantiate();
        instance.name = "OnEnable " + childs.Count.ToString();
    }

    public override void OnDisable()
    {
        Debug.Log("Instantiating GameObject");
        GameObject instance = GameObject.Instantiate();
        instance.name = "OnDisable " + childs.Count.ToString();
    }

    public override void OnStop()
    {
        Debug.Log("Instantiating GameObject");
        GameObject instance = GameObject.Instantiate();
        instance.name = "onStop " + childs.Count.ToString();
    }

}
