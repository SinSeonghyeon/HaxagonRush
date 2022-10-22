#pragma once

struct MoveInfo
{
	enum MoveType
	{
		MoveBy,
		MoveTo,
		MoveFormBy,
		MoveFromTo
	};
	enum LineType
	{
		Linear,
		Bezier,
		Shake
	};

	enum LookType
	{
		Up,
		Look
	};

	std::shared_ptr<Muscle::Transform> _Transform; // ������ �༮.

	Vector3 _vectorBy = Vector3::Zero;
	Vector3 _vectorTo = Vector3::Zero;
	Vector3 _vectorFrom = Vector3::Zero;

	/// ������ � V2 = V1+_bezierVector1, V3 = V4+_bezierVector2
	Vector3 _bezierVector1 = Vector3::Zero;
	Vector3 _bezierVector2 = Vector3::Zero;

	Vector3 _vectorTarget;
	bool _isOrientToPath = false; // ����������� Look�Ѵ�..!


	MoveType _moveType = MoveType::MoveBy;
	LineType _lineType = LineType::Linear;
	LookType _LookType = LookType::Look;

	float _time = 0;
	float _delay = 0;
	float _power = 0.1f;

	bool Update();

	Vector3 _intialPos = Vector3::Zero;

	bool _isRetureInit = false;

	std::shared_ptr<Muscle::Transform> _LookTarget; // Look�� �༮.

private:
	Vector3 BeizerCurve();
	Vector3 LinearLine();
	Vector3 ShakeMove();

	Vector3 _NowPos = Vector3::Zero;

	float _startTime = 0;
	float s = 0;
	float t = 0;

	bool _isStart = false;

	


};

struct LookInfo
{
	std::shared_ptr<Muscle::Transform> _Transform; // ������ �༮.

	Vector3 _LookTargetVector;

	std::shared_ptr<Muscle::Transform> _LookTarget; // Look�� �༮.

	float _speed = 1;

	float _time = 0;

	float _delay = 0;

	Vector3 _offset = Vector3::Zero;

	bool Update();
private:
	float _startTime = 0;

	float t = 0;

	bool _isStart = false;

	Vector3 _startLook = Vector3::Zero;
};
class ObjectMover : public Muscle::IComponents
{
public:
	ObjectMover(std::shared_ptr<Muscle::GameObject> gameObject);

	virtual ~ObjectMover();

private:
	static std::shared_ptr<ObjectMover> _Instance;

	std::list<MoveInfo> _moveInfo; // ����Ʈ�� ������ ���߿� ������ �߰��� �����Ӱ� �ϱ�����!.

	std::list<LookInfo> _LookInfo; // ����Ʈ�� ������ ���߿� ������ �߰��� �����Ӱ� �ϱ�����!.

public:
	static std::shared_ptr<ObjectMover> Get();

	void MoveBy(std::shared_ptr<Muscle::Transform> transform, Vector3 vectorBy, float time, float delay = 0, MoveInfo::LineType _lineType = MoveInfo::Linear, Vector3 bezierVector1 = Vector3(0.0f, 10.0f, 0.0f), Vector3 bezierVector2 = Vector3(0.0f, 10.0f, 0.0f), MoveInfo::LookType looktype = MoveInfo::Look, bool isOrientToPath = false, Vector3 vectorTarget = Vector3(), std::shared_ptr<Muscle::Transform> lookTarget = nullptr); // ��ü�� ������ ��ġ���� ���ϰų� ���� �̵���Ų��.
	void MoveTo(std::shared_ptr<Muscle::Transform> transform, Vector3 vectorTo, float time, float delay = 0, MoveInfo::LineType _lineType = MoveInfo::Linear, Vector3 bezierVector1 = Vector3(0.0f, 10.0f, 0.0f), Vector3 bezierVector2 = Vector3(0.0f, 10.0f, 0.0f), MoveInfo::LookType looktype = MoveInfo::Look, bool isOrientToPath = false, Vector3 vectorTarget = Vector3(), std::shared_ptr<Muscle::Transform> lookTarget = nullptr); // ��ü�� ��ǥ �������� �̵���Ų��.
	void MoveFromBy(std::shared_ptr<Muscle::Transform> transform, Vector3 vectorFrom, Vector3 vectorBy, float time, float delay = 0, MoveInfo::LineType _lineType = MoveInfo::Linear, Vector3 bezierVector1 = Vector3(0.0f, 10.0f, 0.0f), Vector3 bezierVector2 = Vector3(0.0f, 10.0f, 0.0f), MoveInfo::LookType looktype = MoveInfo::Look, bool isOrientToPath = false, Vector3 vectorTarget = Vector3(), std::shared_ptr<Muscle::Transform> lookTarget = nullptr); // ��ü�� ���� �������� �̵���Ų��.
	void MoveFromTo(std::shared_ptr<Muscle::Transform> transform, Vector3 vectorFrom, Vector3 vectorTo, float time, float delay = 0, MoveInfo::LineType _lineType = MoveInfo::Linear, Vector3 bezierVector1 = Vector3(0.0f, 10.0f, 0.0f), Vector3 bezierVector2 = Vector3(0.0f, 10.0f, 0.0f), MoveInfo::LookType looktype = MoveInfo::Look, bool isOrientToPath = false, Vector3 vectorTarget = Vector3(), std::shared_ptr<Muscle::Transform> lookTarget = nullptr); // ��ü�� ���� �������� ��ǥ �������� �̵���Ų��.

	void Shake(std::shared_ptr<Muscle::Transform> transform, float time, float power = 0.2f, float delay = 0, bool isReturnInit = false);

	void LinearLook(std::shared_ptr<Muscle::Transform> transform, float time, std::shared_ptr<Muscle::Transform> lookTarget = nullptr, Vector3 lookTargetVector = Vector3::Zero, float _speed = 1, float delay = 0, Vector3 offset = Vector3::Zero);

	void DeleteObject(std::shared_ptr<Muscle::GameObject> gameObject); // ���� �Ŵ������� ������Ʈ�� �����ϸ� ������ ������Ʈ�� �����ؾ��Ѵ�..

	virtual void LateUpdate(); // ��� Object Udpate���� �� (ObjectManager) ������Ʈ�� Components�� ������ ������ ���� ������Ʈ ���ֱ�.

	virtual void Finalize();
};

